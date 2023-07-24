#include "gtest/gtest.h"
#include "soln/14.h"
#include "soln/1.h"
#include "inputs.h"


std::vector<std::string> ANS = {
    "",    
    "",
    "fl",
    "ab",
    "a",
    "",
    "a",
    "c"
};

std::vector<std::vector<int>> ANS_1 = {
    {0,1},
    {1,2},
    {0,1}
};

TEST(soln1, hash_map1) {
    Solution1 sol1;

    for(int i = 0; i<input1.size(); i++ ){
        EXPECT_EQ(sol1.twoSum(input1[i].first, input1[i].second), ANS_1[i]);
    }
}

TEST(soln14, returnStrings) {
    Solution14 sol14;

    for(int i = 0; i<input14.size(); i++ ){
        EXPECT_EQ(sol14.longestCommonPrefix(input14[i]), ANS[i]);
    }
}