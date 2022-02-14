#include <iostream>
#include <vector>

#include "DataStructures/src/lib/hello/hello.h"

#include "glog/logging.h"
#include "glog/stl_logging.h"



int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);
  Hello s;
  FLAGS_alsologtostderr = true;
  LOG(INFO) << "Running PrintHelloWorld Function ";
  std::cout << s.PrintHelloWorld() << std::endl;
  return 0;
}
