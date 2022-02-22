#ifndef FINDLEVEL_H
#define FINDLEVEL_H

#include <vector>

class BFSFindLevel {
public:
    BFSFindLevel();
    int allLevels(std::vector<std::vector<int>> grid, int x, int y);
};

#endif