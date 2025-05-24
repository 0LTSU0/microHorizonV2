#include <gtest/gtest.h>
#include <SharedData.h>
#include <posMatcherImpl.h>

int addints(int a, int b) {
    return a + b;
}

TEST(HorizonGeneratorTests, chooseFromTwoBestCandidates1) {
    EXPECT_EQ(addints(2, 3), 10);
    EXPECT_EQ(addints(10, 20), 30);
}