#include <posMatcherImpl.h>
#include <cmath>
#include <numbers>
#include <tracer.h>
#include <string>
#include <cmath>

double toRad(double degrees) {
	return degrees * (std::numbers::pi / 180);
}

double toDeg(double rads) {
	return rads * (180 / std::numbers::pi);
}

double distance(const Point& a, const Point& b) {
	return sqrt(pow(a.lat - b.lat, 2) + pow(a.lon - b.lon, 2));
}

// Function to compute the perpendicular distance from a point to a road segment (from chatgpt probably)
double pointToSegmentDistance(const sharedData::inputPosition& p, const Segment& seg) {
	Point ip = { p.lat, p.lon };
	double x1 = seg.start.lat, y1 = seg.start.lon;
	double x2 = seg.end.lat, y2 = seg.end.lon;
	double x0 = p.lat, y0 = p.lon;

	double dx = x2 - x1;
	double dy = y2 - y1;
	double lengthSquared = dx * dx + dy * dy;

	if (lengthSquared == 0) return distance(ip, seg.start); // If segment is a point

	// Projection formula
	double t = ((x0 - x1) * dx + (y0 - y1) * dy) / lengthSquared;
	t = std::max(0.0, std::min(1.0, t)); // Clamp t to [0,1]

	Point projection = { x1 + t * dx, y1 + t * dy };
	return distance(ip, projection);
}

static double calculateHeading(double lat1rad, double lon1rad, double lat2rad, double lon2rad) {
	double delta_lon = lon2rad - lon1rad;
	double x = std::sin(delta_lon) * std::cos(lat2rad);
	double y = std::cos(lat1rad) * std::sin(lat2rad) - std::sin(lat1rad) * std::cos(lat2rad) * std::cos(delta_lon);
	double initialBearing = toDeg(std::atan2(x, y));
	return std::fmod(initialBearing + 360, 360);
}

/*
When positive Y axis corresponds to heading 0, this calculates the "heading" of given segment
in both ways as there really isn't a way to know which way a road is modelled in OSM data 
and we might be driving along it to either direction.

TODO: Check if this math actually makes any sense... When you plot latlon coordinates on 2d surface,
the result is quite distored and I'm not sure if this accounts for that. This logic is same as
plist_to_udp_sender.py but that might be incorrect also. The answer really depends on how GPS receivers work...
*/
static std::pair<double, double> calculateSegmentHeadings(const Segment &segment) {
	auto lat_start_1 = toRad(segment.start.lat);
	auto lon_start_1 = toRad(segment.start.lon);
	auto lat_end_1 = toRad(segment.end.lat);
	auto lon_end_1 = toRad(segment.end.lon);
	auto headingCandidate1 = calculateHeading(lat_start_1, lon_start_1, lat_end_1, lon_end_1);
	double headingCandidate2 = 0.0;

	// the other option is 180 degrees less or more
	if (headingCandidate1 > 180) {
		headingCandidate2 = headingCandidate1 - 180;
	}
	else {
		headingCandidate2 = headingCandidate2 + 180;
	}

	return { headingCandidate1, headingCandidate2 };
}

static double getSmallestHeadingDiff(const std::pair<double, double>& segHeadings, const sharedData::inputPosition& pos) {
	double diff1 = std::fabs(segHeadings.first - pos.heading);
	double diff2 = std::fabs(segHeadings.second - pos.heading);
	if (diff1 < diff2) {
		return diff1;
	}
	return diff2;
}

/* 
TODO: This logic for matching position to road REALLY needs to be implemented better as this works "most of the time"
AS LONG AS roads are relatively sparse and there are not too much overlaps etc.
Current logic:
	1. Find 2 closest roads to current position
	2. IF matching distance to those two is less than tooNearThreshold, select the one that corresponds better to heading in input position
*/
sharedData::RoadInfo matchPosition(const sharedData::inputPosition& pos, std::shared_ptr<sharedData::SharedData> data)
{
	// lock map data mutex to make sure map loader doesn't touch the map data while we're trying to match position
	std::lock_guard<std::mutex> guard(data->mapDataMutex);
	
	constexpr double tooNearThreshold = 0.0001;
	sharedData::RoadInfo bestMatch;
	sharedData::RoadInfo secondBestMatch;
	double bestMatchMatchingDist = std::numeric_limits<double>::max();
	double secondBestMatchMatchingDist = std::numeric_limits<double>::max();
	Segment bestMatchSegment;
	Segment secondBestMatchSegment;

	Segment currSeg;
	for (const sharedData::RoadInfo& road : data->mapData)
	{
		for (size_t i = 0; i < road.nodes.size() - 1; i++)
		{
			currSeg.start = { road.nodes[i].lat(), road.nodes[i].lon() };
			currSeg.end = { road.nodes[i + 1].lat(), road.nodes[i + 1].lon() };
			double d = pointToSegmentDistance(pos, currSeg);
			bool iterChangedResults = true;
			
			if (d < bestMatchMatchingDist)
			{
				// match is better than bestmatch result -> Set as best and move old best to be secondBest
				secondBestMatch = bestMatch;
				secondBestMatchMatchingDist = bestMatchMatchingDist;
				secondBestMatchSegment = bestMatchSegment;
				bestMatchMatchingDist = d;
				bestMatch = road;
				bestMatchSegment = currSeg;
			}
			else if (d < secondBestMatchMatchingDist && d > bestMatchMatchingDist) {
				// match is better than secondBest But worse than best -> set this match as second best
				secondBestMatch = road;
				secondBestMatchMatchingDist = d;
				secondBestMatchSegment = currSeg;
			}
			else {
				// match was worse than the old 1. and 2. best so nothing changed
				iterChangedResults = false;
			}

			if (iterChangedResults) {
				Tracer::log("Closest road is now: " + bestMatch.name + ", id: " + std::to_string(bestMatch.id) + ", dist: " + std::to_string(bestMatchMatchingDist) + 
					" | Second closest road is now: " + secondBestMatch.name + ", id: " + std::to_string(secondBestMatch.id) + ", dist: " + std::to_string(secondBestMatchMatchingDist),
					traceLevel::DEBUG);
			}
		}
	}

	// If the best and second best candidates are too near, select the one that aligns better with input pos
	if (std::fabs(secondBestMatchMatchingDist - bestMatchMatchingDist) <= tooNearThreshold)
	{
		return chooseFromTwoBestCandidates(pos, bestMatchSegment, secondBestMatchSegment, bestMatch, secondBestMatch);
	}

	return bestMatch;
}

/*
As the main matchPosition() implementation, this needs to be improved. Currently only chooses 
the one that aligns better with the heading of our input position OR if the heading correspondances
are very close (i.e. there are two parellel roads, then it chooses the nearer one.
*/
sharedData::RoadInfo chooseFromTwoBestCandidates(const sharedData::inputPosition& pos, const Segment& bestSeg, const Segment& secondSeg, const sharedData::RoadInfo& bestRoad, const sharedData::RoadInfo& secondRoad)
{
	auto bestSegHeadings = calculateSegmentHeadings(bestSeg);
	auto secondBestHeadings = calculateSegmentHeadings(secondSeg);
	double headingDiffToBest = getSmallestHeadingDiff(bestSegHeadings, pos);
	double headingDiffToSecond = getSmallestHeadingDiff(secondBestHeadings, pos);

	// if difference between best and second best diffs is less than 10degrees, return the road that was better according to distance
	if (std::fabs(headingDiffToBest - headingDiffToSecond) < 10) {
		return bestRoad;
	}

	if (headingDiffToBest > headingDiffToSecond) {
		return secondRoad;
	}
	return bestRoad;
}

bool isRoadInfoValid(const sharedData::RoadInfo& road)
{
	// TODO: implement some proper logic. For now let's assume that if road has some nodes then its probaly valid
	return road.nodes.size() > 0;
}
