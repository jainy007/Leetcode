#include "findLevel.h"

#include <iostream>
#include <queue>

BFSFindLevel::BFSFindLevel() {}

int BFSFindLevel::allLevels(std::vector<std::vector<int>> grid, int startx, int endx) {
    // defining all NODES as 1 in the grid matrix
    // if the grid is visited, defining it as 2
    // pushing the grid start point value as 2

    if (grid.empty()) return 0;

    // grid[startx][endx] = 2;
    int m = grid.size(), n = grid[0].size();
    int levels = 0, nodes = 0, cnt = 0;
    std::queue<std::pair<int, int>> q;

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (grid[i][j] != 0) nodes++;
            if (grid[i][j] == 2) q.push({i, j});
        }
    }

    // 4-directional movements
    int dx[4] = {0, 0, 1, -1};  // left right up down
    int dy[4] = {1, -1, 0, 0};

    while (!q.empty()) {
        int k = q.size();
        cnt += k;
        while (k--) {
            int x = q.front().first, y = q.front().second;
            q.pop();
            for (int i = 0; i < 4; i++) {
                int nx = x + dx[i], ny = y + dy[i];
                if (nx < 0 || ny < 0 || nx >= m || ny >= n || grid[nx][ny] != 1) continue;
                grid[nx][ny] = 2;
                q.push({nx, ny});
            }
        }
        if (!q.empty()) levels++;
    }
    return nodes == cnt ? levels : -1;
}
