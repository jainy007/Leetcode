load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_test")

cc_test(
    name = "hello_test",
    srcs = ["hello_test.cc"],
    deps = [
        "//DataStructures/src/lib/hello:hello",
        "@com_google_googletest//:gtest_main",
    ],
)
