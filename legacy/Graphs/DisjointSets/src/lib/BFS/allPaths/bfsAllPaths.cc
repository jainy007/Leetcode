#include "bfsAllPaths.h"

#include <iostream>
#include <queue>

BFSAllPaths::BFSAllPaths() {}

std::vector<std::vector<int>> BFSAllPaths::allPaths(std::vector<std::vector<int>> graph) {
    std::vector<std::vector<int>> paths;
    std::vector<int> path;
    // print adjacency list
    for (int i = 0; i < graph.size(); i++) {
        std::cout << i << "---";
        for (auto j : graph[i]) std::cout << j << " ";
        std::cout << std::endl;
    }

    // create queue for BFS
    std::queue<std::vector<int>> q;
    path.push_back(0);
    q.push(path);

    // create a visited/seen container (NOT REQUIRED WHEN FINDING ALL PATHS)
    // std::vector<bool> seen(n);
    // seen[src] = true;

    // Recursive Queue Traversal untill empty
    while (!q.empty()) {
        std::vector<int> currentPath = q.front();
        q.pop();

        int node = currentPath.back();

        // Add all neighbors to Queue
        for (int neighbor : graph[node]) {
            // copy currentPath to temp vector
            std::vector<int> temp(currentPath.begin(), currentPath.end());
            temp.push_back(neighbor);
            if (neighbor == graph.size() - 1)
                paths.push_back(temp);
            else
                q.push(temp);
        }
    }
    return paths;
}