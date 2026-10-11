#include <vector>
#include <queue>
#include <iostream>
#include<bits/stdc++.h>

using namespace std;

void Dijkstra(int n, vector<vector<int>> edgeList, int source) {
    // create adjacency List 
    vector<vector<pair<int, int>>> adjacency_list(n+1);
    for (vector<int> edge : edgeList) {
        adjacency_list[edge[0]].push_back(make_pair(edge[1], edge[2]));
    }
    // create distance List
    vector<int> dis(n+1, INT_MAX);
    dis[source] = 0; 
    
    priority_queue<pair<int, int>> pq;
    pq.push({0, source});
    
    while(!pq.empty()) {
        int c_dis = pq.top().first;
        int c_node = pq.top().second;
        pq.pop();
        for(auto edge : adjacency_list[c_node]) {
            // relaxation on current neighbor of c_node
            if(dis[edge.first] > dis[c_node]+edge.second) {
                dis[edge.first] = dis[c_node] + edge.second;
                pq.push({dis[edge.first],edge.first});
            }
        }
    }
    for(auto i: dis)
        cout<<i<<" ";
    cout<<endl;
}

int main() {
    vector<vector<int>> testList{
        {1,2,1},
        {3,1,3},
        {2,3,2}
    }; // INT_MAX 0 1 3
    
    vector<vector<int>> edgeList1{ 
        {1,2,50},
        {2,3,10},
        {1,3,45},
        {4,1,10},
        {4,5,15},
        {6,5,3},
        {3,5,30},
        {5,3,35},
        {5,2,20},
        {1,4,10},
        {2,4,15}
    }; // INT_MAX 0 45 45 10 25 INT_MAX 

    Dijkstra(3, testList, 1);
    Dijkstra(6, edgeList1, 1);
}