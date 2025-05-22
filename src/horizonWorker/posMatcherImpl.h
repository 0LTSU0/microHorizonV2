#pragma once

#include <SharedData.h>

sharedData::RoadInfo matchPosition(const sharedData::inputPosition&, std::shared_ptr<sharedData::SharedData>);
bool isRoadInfoValid(const sharedData::RoadInfo&); //Check if given RoadData seems valid
