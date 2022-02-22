// https://leetcode.com/explore/learn/card/graph/618/disjoint-set/3879/
/*
                      Union-find Constructor	QUICK-Find	  Union	      Connected
Time Complexity	  O(N)                      O(LogN)	      O(LogN)	  O(LogN)
*/

#include "unionFind.h"
#include <algorithm>

UnionFind::UnionFind(int sz) : root(sz), rank(sz) {
    for (int i = 0; i < sz; i++) {
        root[i] = i;
        rank[i] = 1;
    }
    std::cout << "unionFind Operation" << std::endl;
    for (int i : root) std::cout << i << " ";
    std::cout << "\n";
}

int UnionFind::find(int x) {
    while (x != root[x]) {
        x = root[x];
    }
    std::cout << "Find Operation" << std::endl;
    std::cout << "x : " << x << " and "
              << "root of x : " << root[x] << std::endl;
    return x;
}

void UnionFind::unionSet(int x, int y) {
    int rootX = find(x);
    int rootY = find(y);
    if (rootX != rootY) {
        if (rank[rootX] > rank[rootY]) {
            root[rootY] = rootX;
        } else if (rank[rootX] < rank[rootY]) {
            root[rootX] = rootY;
        } else {
            root[rootY] = rootX;
            rank[rootX] += 1;
        }
    }
    std::cout << "unionSet Operation" << std::endl;

    for (std::size_t i = 0; i < root.size(); i++) {
        std::cout << i << " ";
        std::cout << root[i] << " " << std::endl;
    }
}

bool UnionFind::connected(int x, int y) {
    return find(x) == find(y);
}
