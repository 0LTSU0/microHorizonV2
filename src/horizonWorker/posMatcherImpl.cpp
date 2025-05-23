#include <posMatcherImpl.h>
#include <cmath>
#include <numbers>
#include <tracer.h>

struct Point {
	double lat, lon;
};

struct Segment {
	Point start, end;
};

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


sharedData::RoadInfo matchPosition(const sharedData::inputPosition& pos, std::shared_ptr<sharedData::SharedData> data)
{
	// lock map data mutex to make sure map loader doesn't touch the map data while we're trying to match position
	std::lock_guard<std::mutex> guard(data->mapDataMutex);

	sharedData::RoadInfo bestMatch;
	double minDist = std::numeric_limits<double>::max();
	Segment currSeg;
	for (const sharedData::RoadInfo& road : data->mapData)
	{
		for (size_t i = 0; i < road.nodes.size() - 1; i++)
		{
			currSeg.start = { road.nodes[i].lat(), road.nodes[i].lon() };
			currSeg.end = { road.nodes[i + 1].lat(), road.nodes[i + 1].lon() };
			double d = pointToSegmentDistance(pos, currSeg);
			if (d < minDist)
			{
				minDist = d;
				bestMatch = road;
				Tracer::log("Closest road is: " + bestMatch.name + " | dist: " + std::to_string(d), traceLevel::DEBUG);
			}
		}
	}

	return bestMatch;
}

bool isRoadInfoValid(const sharedData::RoadInfo& road)
{
	// TODO: implement some proper logic. For now let's assume that if road has some nodes then its probaly valid
	return road.nodes.size() > 0;
}