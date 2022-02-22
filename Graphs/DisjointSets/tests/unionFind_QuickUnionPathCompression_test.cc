#include "Graphs/DisjointSets/src/lib/UnionFind/QuickUnionPathCompression/unionFind.h"

#include "gtest/gtest.h"

TEST(unionFindQuickUnionPathCompressionTest, ReturnTrueFalse) {
    UnionFind uf(10);
    // build a tree <1-2-5-6-7 3-8-9 4>
    uf.unionSet(1, 2);
    uf.unionSet(2, 5);
    uf.unionSet(5, 6);
    uf.unionSet(6, 7);
    uf.unionSet(3, 8);
    uf.unionSet(8, 9);

    EXPECT_EQ(uf.connected(1, 5), true);
    EXPECT_EQ(uf.connected(5, 7), true);
    EXPECT_EQ(uf.connected(4, 9), false);
    // connect 4 to 9 <1-2-5-6-7  3-8-9-4>
    uf.unionSet(9, 4);
    EXPECT_EQ(uf.connected(4, 9), true);
}
