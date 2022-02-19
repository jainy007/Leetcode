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
        "//Graphs/DisjointSets/src/lib:DFS",
        "@com_google_googletest//:gtest_main",
    ],
)
