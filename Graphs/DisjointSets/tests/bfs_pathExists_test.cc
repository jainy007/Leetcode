#include "Graphs/DisjointSets/src/lib/BFS/validPath/bfsValidPath.h"

#include <vector>

#include "gtest/gtest.h"

TEST(bfsPathExistsTest, ReturnTrueFalse) {
    BFSValidPath bfs;

    std::vector<std::vector<int>> edges{{0, 1}, {0, 2}, {3, 5}, {5, 4}, {4, 3}};

    EXPECT_EQ(bfs.validPath(6, edges, 0, 5), false);
    EXPECT_EQ(bfs.validPath(6, edges, 1, 2), true);
    EXPECT_EQ(bfs.validPath(6, edges, 3, 5), true);
}
