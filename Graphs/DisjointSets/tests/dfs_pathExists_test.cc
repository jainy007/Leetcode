#include "Graphs/DisjointSets/src/lib/DFS/validPath/dfsValidPath.h"
#include "Graphs/DisjointSets/src/lib/UnionFind/QuickUnionPathCompression/unionFind.h"

#include <vector>

#include "gtest/gtest.h"

TEST(dfsPathExistsTest, ReturnTrueFalse) {
    Dfs dfs;

    std::vector<std::vector<int>> edges{{0, 1}, {0, 2}, {3, 5}, {5, 4}, {4, 3}};

    EXPECT_EQ(dfs.ValidPath(6, edges, 0, 5), false);
    EXPECT_EQ(dfs.ValidPath(6, edges, 1, 2), true);
    EXPECT_EQ(dfs.ValidPath(6, edges, 3, 5), true);
}

TEST(unionFindPathExistsTest, ReturnTrueFalse) {
    UnionFind uf(6);

    uf.unionSet(0, 1);
    uf.unionSet(0, 2);
    uf.unionSet(3, 5);
    uf.unionSet(5, 4);
    uf.unionSet(4, 3);

    EXPECT_EQ(uf.connected(0, 5), false);
    EXPECT_EQ(uf.connected(1, 2), true);
    EXPECT_EQ(uf.connected(3, 5), true);
}