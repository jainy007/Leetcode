#include "Graphs/DisjointSets/src/lib/BFS/levelOrderTraversal/findLevel.h"

#include <iostream>
#include <vector>
#include "gtest/gtest.h"

TEST(bfsfindLevelTest, ReturnallLevels) {
    BFSFindLevel flvl;

    std::vector<std::vector<int>> grid{{2, 1, 1}, {1, 1, 0}, {0, 1, 1}};

    std::vector<std::vector<int>> grid2{{2, 1}, {0, 1}};

    std::vector<std::vector<int>> grid3{{1, 1}, {0, 1}};
    // a 3x3 matrix has 5 levels, return will be 4
    /*
    1) Algorithm will traverse each node until it reaches the farthest end of the graph
    with shortest path.
    2) it can only traverse left, right, top and bottom (4-directional)
    3) all 1's in the graph are nodes
    */

    // std::cout<<fl.allLevels(grid, 0, 0);
    EXPECT_EQ(flvl.allLevels(grid, 0, 0), 4);
    EXPECT_EQ(flvl.allLevels(grid2, 0, 0), 2);
    EXPECT_EQ(flvl.allLevels(grid3, 0, 0), -1);
    EXPECT_EQ(flvl.allLevels({}, 0, 0), 0);
}
