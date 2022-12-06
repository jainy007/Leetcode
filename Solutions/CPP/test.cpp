#include "gtest/gtest.h"
#include "soln/14.h"
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

TEST(soln14, returnStrings) {
    Solution sol14;

    for(int i = 0; i<input14.size(); i++ ){
        EXPECT_EQ(sol14.longestCommonPrefix(input14[i]), ANS[i]);
    }
}