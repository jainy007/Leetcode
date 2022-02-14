#include "DataStructures/src/lib/hello/hello.h"

#include <map>
#include <vector>

#include "gtest/gtest.h"

TEST(HelloTest, ReturnHelloWorld) {
  Hello hello;
  std::string actual = hello.PrintHelloWorld();
  std::string expected = "**** Hello World ****";
  EXPECT_EQ(expected, actual);
}
