#pragma once

#include <SharedData.h>

struct Point {
	double lat = 0.0, lon = 0.0;
};

struct Segment {
	Point start, end;
};

sharedData::RoadInfo matchPosition(const sharedData::inputPosition&, std::shared_ptr<sharedData::SharedData>);
sharedData::RoadInfo chooseFromTwoBestCandidates(const sharedData::inputPosition&, const Segment&, const Segment&, sharedData::RoadInfo&, sharedData::RoadInfo&);
bool isRoadInfoValid(const sharedData::RoadInfo&); //Check if given RoadData seems valid
