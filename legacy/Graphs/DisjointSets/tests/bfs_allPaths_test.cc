#include <algorithm>
#include <iostream>
#include <vector>
#include "Graphs/DisjointSets/src/lib/BFS/allPaths/bfsAllPaths.h"
#include "gtest/gtest.h"

class bfsallPaths {
public:
    bool getResult(std::vector<int> n1, std::vector<std::vector<int>> v) {
        return std::find(v.begin(), v.end(), n1) != v.end();
    }
};

TEST(bfsallPathsTest, ReturnAllPaths) {
    BFSAllPaths bfs;
    bfsallPaths d;

    std::vector<std::vector<int>> graph1{{1, 2}, {3}, {3}, {}};
    std::vector<std::vector<int>> graph2{{4, 3, 1}, {3, 2, 4}, {3}, {4}, {}};

    std::vector<std::vector<int>> output1{{0, 1, 3}, {0, 2, 3}};
    std::vector<std::vector<int>> output2{{0, 4}, {0, 3, 4}, {0, 1, 3, 4}, {0, 1, 2, 3, 4}, {0, 1, 4}};

    std::vector<std::vector<int>> actualOutput1;
    std::vector<std::vector<int>> actualOutput2;

    actualOutput1 = bfs.allPaths(graph1);
    EXPECT_EQ(output1.size(), actualOutput1.size());

    actualOutput2 = bfs.allPaths(graph2);
    EXPECT_EQ(output2.size(), actualOutput2.size());

    for (auto i : actualOutput1) {
        EXPECT_EQ(d.getResult(i, output1), true);
    }
    for (auto i : actualOutput2) {
        EXPECT_EQ(d.getResult(i, output2), true);
    }
}
