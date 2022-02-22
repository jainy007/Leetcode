load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_test")

cc_test(
    name = "hello_test",
    srcs = ["hello_test.cc"],
    deps = [
        "//Graphs/DisjointSets/src/lib:hello",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "unionFind_QuickFind_test",
    srcs = ["unionFind_QuickFind_test.cc"],
    deps = [
        "//Graphs/DisjointSets/src/lib:QuickFind",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "unionFind_QuickUnion_test",
    srcs = ["unionFind_QuickUnion_test.cc"],
    deps = [
        "//Graphs/DisjointSets/src/lib:QuickUnion",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "unionFind_QuickUnionRank_test",
    srcs = ["unionFind_QuickUnionRank_test.cc"],
    deps = [
        "//Graphs/DisjointSets/src/lib:QuickUnionRank",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "unionFind_QuickUnionPathCompression_test",
    srcs = ["unionFind_QuickUnionPathCompression_test.cc"],
    deps = [
        "//Graphs/DisjointSets/src/lib:QuickUnionPathCompression",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "unionRank_PathCompression_test",
    srcs = ["unionRank_PathCompression_test.cc"],
    deps = [
        "//Graphs/DisjointSets/src/lib:QuickUnionRankPathCompression",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "dfs_pathExists_test",
    srcs = ["dfs_pathExists_test.cc"],
    deps = [
        "//Graphs/DisjointSets/src/lib:DFSvalidPath",
        "//Graphs/DisjointSets/src/lib:QuickUnionPathCompression",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "dfs_allPaths_test",
    srcs = ["dfs_allPaths_test.cc"],
    deps = [
        "//Graphs/DisjointSets/src/lib:DFSallPaths",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "bfs_pathExists_test",
    srcs = ["bfs_pathExists_test.cc"],
    deps = [
        "//Graphs/DisjointSets/src/lib:BFSvalidPath",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "bfs_allPaths_test",
    srcs = ["bfs_allPaths_test.cc"],
    deps = [
        "//Graphs/DisjointSets/src/lib:BFSallPaths",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "bfs_allLevel_test",
    srcs = ["bfs_allLevel_test.cc"],
    deps = [
        "//Graphs/DisjointSets/src/lib:BFSfindLevel",
        "@com_google_googletest//:gtest_main",
    ],
)
