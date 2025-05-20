#pragma once

#include <queue>
#include <vector>
#include <mutex>
#include <string>
#include <atomic>

// TODO: cleanup these (should incluce only things we need for RoadInfo struct and include rest in mapLoadWorker)
#define NOMINMAX //fixes compilation errors from osmium on windows
#include <osmium/io/reader.hpp>
#include <osmium/io/pbf_input.hpp>
#include <osmium/io/xml_input.hpp>
#include <osmium/handler.hpp>
#include <osmium/visitor.hpp>
#include <osmium/osm/box.hpp>
#include <osmium/osm/way.hpp>
#include <osmium/index/map/flex_mem.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>

namespace sharedData {

class inputPosition {
public:
	double lat = 0.0;
	double lon = 0.0;
	double heading = 0.0;
	double speed = 0.0;
	bool isValidObs() {
		if (lat == 0.0 || lon == 0.0) // If coordinates at 0.0 then obs invalid for sure
		{
			return false;
		}
		if (heading < 0 || heading > 360) // heading should be in range of 0-360
		{
			return false;
		}
		return true;
	};
};

struct RoadInfo {
	osmium::object_id_type id;
	std::string name;
	std::string highway_type;
	std::vector<osmium::Location> nodes;
};

enum RoadLoaderState {
	NOT_INITIALIZED,
	LOADING_MAP,
	IDLE
};

struct SharedData {
	std::queue<inputPosition> incomingPositions;
	std::vector<RoadInfo> mapData;

	std::mutex inputPosMutex;
	std::mutex mapDataMutex;
	std::mutex roadLoaderStateMutex;

	std::atomic<bool> initialValidPosReceived{ false };
	std::atomic<bool> initialMapLoadDone{ false };
	std::atomic<bool> appIsRunning{ true };
	std::atomic<RoadLoaderState> roadLoaderState{ RoadLoaderState::NOT_INITIALIZED };
};

} // namespace sharedData