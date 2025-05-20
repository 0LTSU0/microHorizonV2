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

struct inputPosition {
	double lat;
	double lon;
	double heading;
};

struct RoadInfo {
	osmium::object_id_type id;
	std::string name;
	std::string highway_type;
	std::vector<osmium::Location> nodes;
};

struct SharedData {
	std::queue<inputPosition> incomingPositions;
	std::vector<RoadInfo> mapData;

	int test_var = 0;

	std::atomic<bool> initialValidPosReceived{ false };
	std::atomic<bool> initialMapLoadDone{ false };
};

} // namespace sharedData