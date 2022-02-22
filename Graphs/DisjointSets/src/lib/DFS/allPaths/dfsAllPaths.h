#ifndef ALL_PATHS_H
#define ALL_PATHS_H

#include <vector>

class Dfs {
public:
    Dfs();
    void dfsGraph(std::vector<std::vector<int>>& graph, int node, std::vector<int>& path,
                  std::vector<std::vector<int>>& paths);
    std::vector<std::vector<int>> allPaths(std::vector<std::vector<int>>& graph);
};

#endif