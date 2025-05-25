#include <memory>

#include <gtest/gtest.h>
#include <SharedData.h>
#include <posMatcherImpl.h>

using namespace sharedData;

int addints(int a, int b) {
    return a + b;
}

//TBD
std::shared_ptr<SharedData> initShareDataForPosMatcher1() {
    std::shared_ptr<SharedData> sharedData = std::make_shared<SharedData>();
    
    // Part of Kasarmintie
    std::vector<osmium::Location> roadNodes;
    roadNodes.push_back(osmium::Location(65.018443, 25.523300));
    roadNodes.push_back(osmium::Location(65.019229, 25.520988));
    roadNodes.push_back(osmium::Location(65.019645, 25.519679));
    roadNodes.push_back(osmium::Location(65.019907, 25.518557));
    roadNodes.push_back(osmium::Location(65.020220, 25.516126));
    roadNodes.push_back(osmium::Location(65.020517, 25.513337));
    RoadInfo kasarmintie{
        1,
        "Kasarmintie",
        "normalidk",
        roadNodes
    };
    sharedData->mapData.push_back(kasarmintie);

    // Part of värtöntie (branches off from kasarmintie)
    roadNodes.clear();
    roadNodes.push_back(osmium::Location(65.019676, 25.519499));
    roadNodes.push_back(osmium::Location(65.019187, 25.519119));
    roadNodes.push_back(osmium::Location(65.018698, 25.518741));
    roadNodes.push_back(osmium::Location(65.018255, 25.518166));
    RoadInfo vartontie{
        1,
        "Vartontie",
        "normalidk",
        roadNodes
    };
    sharedData->mapData.push_back(vartontie);
    
    return sharedData;
}


// Position in intersection, correct road should be chosen based on heading
// TODO use mocks or something to see if heading is used for result
TEST(HorizonGeneratorTests, chooseFromTwoBestCandidatest1) {
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
TEST(HorizonGeneratorTests, chooseFromTwoBestCandidatest2) {
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
TEST(HorizonGeneratorTests, chooseFromTwoBestCandidatest3) {
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
