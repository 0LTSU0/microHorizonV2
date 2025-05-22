#include <posMatcherImpl.h>

sharedData::RoadInfo matchPosition(const sharedData::inputPosition& pos, std::shared_ptr<sharedData::SharedData> data)
{
	// lock map data mutex to make sure map loader doesn't touch the map data while we're trying to match position
	std::lock_guard<std::mutex> guard(data->mapDataMutex);

	sharedData::RoadInfo bestMatch;
	for (const sharedData::RoadInfo& road : data->mapData)
	{

	}

	return bestMatch;
}

bool isRoadInfoValid(const sharedData::RoadInfo& road)
{
	// TODO: implement some proper logic. For now let's assume that if road has some nodes then its probaly valid
	return road.nodes.size() > 0;
}