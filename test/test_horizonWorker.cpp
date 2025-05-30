#include <memory>
#include <string>
#include <iostream>
#include <vector>

#include <gtest/gtest.h>
#include <SharedData.h> //atm this includes all needed osmium things
#include <posMatcherImpl.h>
#include "map_paths.h"


using namespace sharedData;

//note to self, getting locations for nodes is kinda complicated, good example for that https://github.com/osmcode/libosmium/blob/cf81df16ddd2fbee6eede4e84978130419eef759/examples/osmium_road_length.cpp#L3
using index_type = osmium::index::map::FlexMem<osmium::unsigned_object_id_type, osmium::Location>; //not sure what this does
using location_handler_type = osmium::handler::NodeLocationsForWays<index_type>; //but this is needed to get valid locations on nodes

class HorizonGeneratorTests : public ::testing::Test, public osmium::handler::Handler {
protected:
    void SetUp() override {
        // Initialization code
    }

    void TearDown() override {
        // Cleanup code
    }

    // Read OSM map from given file path
    void readMap(std::string path) {
        std::cout << "Reading map data from " << path << std::endl;
        index_type index;
        location_handler_type location_handler{ index };
        osmium::io::Reader reader(path, osmium::osm_entity_bits::node | osmium::osm_entity_bits::way);
        osmium::apply(reader, location_handler, *this);
    }

    std::shared_ptr<SharedData> m_shareData = std::make_shared<SharedData>();

public:
    void way(const osmium::Way& way) { // see ThirdParty/libosmium-2.20.0/include/osmium/handler.hpp
        std::cout << "Loading road id: " << way.id() << std::endl;

        const char* highway_tag = way.tags()["highway"];
        if (highway_tag && std::string(highway_tag) != "path") {
            std::vector<osmium::Location> road_nodes;
            for (const auto& node : way.nodes()) {
                if (node.location().is_undefined()) {
                    // in case this way had some undefined node location
                    continue;
                }
                road_nodes.push_back(node.location());
            }

            RoadInfo roadInfo{
                way.id(),
                way.tags().get_value_by_key("name", "Unnamed Road"),
                highway_tag,
                road_nodes
            };

            m_shareData->mapData.push_back(roadInfo);
        }
    }
};

// ----------------------------- TEST FULL POSITION MATCHER IMPL ----------------------------------------------

// Position on Haukiputaantie far from intersections
// Expect res to be on Haukiputaantie
TEST_F(HorizonGeneratorTests, mapMatcherTest1) {
    readMap(MAP_SIMPLE_NEAR_HAUKIPUDAS);
    inputPosition inputPos;
    inputPos.lat = 65.151555;
    inputPos.lon = 25.350546;
    inputPos.heading = 340;

    auto res = matchPosition(inputPos, m_shareData);
    EXPECT_EQ(res.id, 321529018);
}

// Position on Haukiputaantie near intersection
// Expect res to be on Haukiputaantie
TEST_F(HorizonGeneratorTests, mapMatcherTest2) {
    readMap(MAP_SIMPLE_NEAR_HAUKIPUDAS);
    inputPosition inputPos;
    inputPos.lat = 65.145201;
    inputPos.lon = 25.352283;
    inputPos.heading = 340;

    auto res = matchPosition(inputPos, m_shareData);
    EXPECT_EQ(res.id, 321529018);
}

// Position on Haukiputaantie near intersection but heading is turning onto Takkurananntie
// Expect res to be on Takkurannantie
TEST_F(HorizonGeneratorTests, mapMatcherTest3) {
    readMap(MAP_SIMPLE_NEAR_HAUKIPUDAS);
    inputPosition inputPos;
    inputPos.lat = 65.145201;
    inputPos.lon = 25.352283;
    inputPos.heading = 100;

    auto res = matchPosition(inputPos, m_shareData);
    EXPECT_EQ(res.id, 168741105);
}

// Position on Saaristonkatu "far" from intersections
// Expect res to be on Saaristonkatu
TEST_F(HorizonGeneratorTests, mapMatcherTest4) {
    readMap(MAP_COMPLEX_OULU_CENTER);
    inputPosition inputPos;
    inputPos.lat = 65.010752;
    inputPos.lon = 25.470106;
    inputPos.heading = 290;

    auto res = matchPosition(inputPos, m_shareData);
    EXPECT_EQ(res.id, 19845843);
}

// Position on Saaristonkatu near intersection
// Expect res to be on Saaristonkatu based on heading
TEST_F(HorizonGeneratorTests, mapMatcherTest5) {
    readMap(MAP_COMPLEX_OULU_CENTER);
    inputPosition inputPos;
    inputPos.lat = 65.010490;
    inputPos.lon = 25.470943;
    inputPos.heading = 290;

    auto res = matchPosition(inputPos, m_shareData);
    EXPECT_EQ(res.id, 19845843);
}

// Position on Saaristonkatu near intersection
// Expect res to be on Isokatu based on heading
TEST_F(HorizonGeneratorTests, mapMatcherTest6) {
    readMap(MAP_COMPLEX_OULU_CENTER);
    inputPosition inputPos;
    inputPos.lat = 65.010490;
    inputPos.lon = 25.470943;
    inputPos.heading = 200;

    auto res = matchPosition(inputPos, m_shareData);
    EXPECT_EQ(res.id, 1288852235);
}


// ----------------------------- TEST chooseFromTwoBestCandidatest2 IMPL ----------------------------------------------


// Position in intersection, correct road should be chosen based on heading
// TODO use mocks or something to see if heading is used for result
TEST_F(HorizonGeneratorTests, chooseFromTwoBestCandidatest1) {
    inputPosition inputPos;
    inputPos.lat = 65.019686;
    inputPos.lon = 25.519523;
    inputPos.heading = 290;
    
    RoadInfo roadCorrect;
    RoadInfo roadWrong;
    roadCorrect.name = "Correct road";
    roadWrong.name = "Wrong road";
    roadCorrect.id = 1;
    roadWrong.id = 69;

    Segment segmentCorrect{ {65.019522, 25.520108}, {65.019963, 25.518179} };
    Segment segmentWrong{ {65.019686, 25.519499}, {65.018945, 25.518982} };

    RoadInfo res = chooseFromTwoBestCandidates(inputPos, segmentCorrect, segmentWrong, roadCorrect, roadWrong);

    EXPECT_EQ(res.id, roadCorrect.id);
    EXPECT_EQ(res.name, roadCorrect.name);
}

// Position in intersection, correct road should be chosen based on heading
// (same as chooseFromTwoBestCandidatest1 but correct/wrong segments are other way around and heading is different)
// TODO use mocks or something to see if heading is used for result
TEST_F(HorizonGeneratorTests, chooseFromTwoBestCandidatest2) {
    inputPosition inputPos;
    inputPos.lat = 65.019686;
    inputPos.lon = 25.519523;
    inputPos.heading = 10;

    RoadInfo roadCorrect;
    RoadInfo roadWrong;
    roadCorrect.name = "Correct road";
    roadWrong.name = "Wrong road";
    roadCorrect.id = 1;
    roadWrong.id = 69;

    Segment segmentCorrect{ {65.019686, 25.519499}, {65.018945, 25.518982} };
    Segment segmentWrong{ {65.019522, 25.520108}, {65.019963, 25.518179} };
    
    RoadInfo res = chooseFromTwoBestCandidates(inputPos, segmentCorrect, segmentWrong, roadCorrect, roadWrong);

    EXPECT_EQ(res.id, roadCorrect.id);
    EXPECT_EQ(res.name, roadCorrect.name);
}

// Two parellel roads. Position is closer to correctRoad/segmentCorrect -> heading stuff should be ignored
// TODO use mocks or something to see if heading is NOT used for result
TEST_F(HorizonGeneratorTests, chooseFromTwoBestCandidatest3) {
    inputPosition inputPos;
    inputPos.lat = 65.036859;
    inputPos.lon = 25.447702;
    inputPos.heading = 45;

    RoadInfo roadCorrect;
    RoadInfo roadWrong;
    roadCorrect.name = "Correct road";
    roadWrong.name = "Wrong road";
    roadCorrect.id = 1;
    roadWrong.id = 69;

    Segment segmentCorrect{ {65.036620,25.446361}, {65.037315, 25.450502} };
    Segment segmentWrong{ {65.037332, 25.445752}, {65.037962, 25.449607} };

    RoadInfo res = chooseFromTwoBestCandidates(inputPos, segmentCorrect, segmentWrong, roadCorrect, roadWrong);

    EXPECT_EQ(res.id, roadCorrect.id);
    EXPECT_EQ(res.name, roadCorrect.name);
}
