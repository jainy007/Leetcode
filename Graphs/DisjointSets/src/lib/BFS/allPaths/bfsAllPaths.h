#ifndef BFSALLPATHS_H
#define BFSALLPATHS_H

#include <vector>

class BFSAllPaths {
public:
    BFSAllPaths();
    std::vector<std::vector<int>> allPaths(std::vector<std::vector<int>> graph);
};

#endif