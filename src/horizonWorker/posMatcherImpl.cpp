#include <posMatcherImpl.h>
#include <cmath>
#include <numbers>
#include <tracer.h>
#include <string>



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
the one that aligns better with the heading of our input position
*/
sharedData::RoadInfo chooseFromTwoBestCandidates(const sharedData::inputPosition& pos, const Segment& bestSeg, const Segment& secondSeg, const sharedData::RoadInfo& bestRoad, const sharedData::RoadInfo& secondRoad)
{
	pos;
	return bestRoad;


}

bool isRoadInfoValid(const sharedData::RoadInfo& road)
{
	// TODO: implement some proper logic. For now let's assume that if road has some nodes then its probaly valid
	return road.nodes.size() > 0;
}
