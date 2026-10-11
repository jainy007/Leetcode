#ifndef BFSVALIDPATH_H
#define BFSVALIDPATH_H

#include <vector>

class BFSValidPath {
public:
    BFSValidPath();
    bool validPath(int n, std::vector<std::vector<int>> edges, int src, int dst);
};

#endif