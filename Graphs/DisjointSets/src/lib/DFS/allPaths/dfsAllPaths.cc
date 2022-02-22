#include "dfsAllPaths.h"

Dfs::Dfs() {}

void Dfs::dfsGraph(std::vector<std::vector<int>>& graph, int node, std::vector<int>& path,
                   std::vector<std::vector<int>>& paths) {
    path.push_back(node);
    if (node == graph.size() - 1) {
        paths.emplace_back(path);
        return;
    }
    std::vector<int> nextNodes = graph[node];
    for (int nextNode : nextNodes) {
        Dfs::dfsGraph(graph, nextNode, path, paths);
        path.pop_back();
    }
}

std::vector<std::vector<int>> Dfs::allPaths(std::vector<std::vector<int>>& graph) {
    std::vector<std::vector<int>> paths;
    if (graph.size() == 0) {
        return paths;
    }
    std::vector<int> path;
    Dfs::dfsGraph(graph, 0, path, paths);
    return paths;
}
