#include "bfsValidPath.h"

#include <iostream>
#include <queue>

BFSValidPath::BFSValidPath() {}

bool BFSValidPath::validPath(int n, std::vector<std::vector<int>> edges, int src, int dst) {
    // create adjacency list
    std::vector<std::vector<int>> adjacency_list(n);
    for (std::vector<int> edge : edges) {
        adjacency_list[edge[0]].push_back(edge[1]);
        adjacency_list[edge[1]].push_back(edge[0]);  // if path is bidirectional
    }
    // print adjacency list
    for (int i = 0; i < adjacency_list.size(); i++) {
        std::cout << i << "---";
        for (auto j : adjacency_list[i]) std::cout << j << " ";
        std::cout << std::endl;
    }

    // create queue for BFS
    std::queue<int> q;
    q.push(src);

    // create a visited/seen container
    std::vector<bool> seen(n);
    seen[src] = true;

    // Recursive Queue Traversal untill empty
    while (!q.empty()) {
        int node = q.front();
        q.pop();

        if (node == dst) return true;

        // Add all neighbors to Queue
        for (int neighbor : adjacency_list[node]) {
            if (!seen[neighbor]) {
                seen[neighbor] = true;
                q.push(neighbor);
            }
        }
    }
    return false;
}
