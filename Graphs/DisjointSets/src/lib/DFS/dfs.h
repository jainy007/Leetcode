#ifndef DFS_H
#define DFS_H

#include <vector>

class Dfs{
public:
    Dfs();
    bool ValidPath(int n, std::vector<std::vector<int>> edges, int start, int end);
    void addSeen(bool x, int node);

};

#endif