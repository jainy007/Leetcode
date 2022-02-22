#include "dfsValidPath.h"

#include <stack>
#include <vector>

Dfs::Dfs() {}

bool Dfs::ValidPath(int n, std::vector<std::vector<int>> edges, int start, int end) {
    std::vector<std::vector<int>> adjacency_list(n);
    for (std::vector<int> edge : edges) {
        adjacency_list[edge[0]].push_back(edge[1]);
        adjacency_list[edge[1]].push_back(edge[0]);
    }

    std::stack<int> st;
    st.push(start);
    std::vector<bool> seen(n);
    while (!st.empty()) {
        // Get the current node.
        int node = st.top();
        st.pop();

        // Check if we have reached the target node.
        if (node == end) {
            return true;
        }

        // Check if we've already visited this node.
        if (seen[node]) {
            continue;
        }
        seen[node] = true;

        // Add all neighbors to the stack.
        for (int neighbor : adjacency_list[node]) {
            st.push(neighbor);
        }
    }
    return false;
}
