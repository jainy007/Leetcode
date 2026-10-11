#include <vector>
#include <algorithm>
#include <bits/stdc++.h>

using namespace std;

void BellManFord(int n, vector<vector<int>> edgeList) {
    //edgeList[i] = {u, v, c(u,v)}
    sort(edgeList.begin(),  edgeList.end());
    vector<int> nodeList(n+1, INT_MAX);                
    nodeList[1] = 0;
    int count = n-1; 
    
    while(count!=0){ 
        for (int i = 0; i<edgeList.size();i++){
            int u = edgeList[i][0];
            int v = edgeList[i][1];
            int c = edgeList[i][2];
            if (nodeList[u] + c < nodeList[v]){
                nodeList[v] = nodeList[u] + c;
            }
        }
        for (auto i : nodeList)
            cout<<i<<" ";
        cout<<endl;
        count-=1;
    }
    cout<<endl;
}

int main() {
    vector<vector<int>> edgeList1{ 
        {1,2,4},
        {1,4,5},
        {3,2,-10},
        {4,3,3}
    }; // INT_MAX 0 -2 8 5
    
    vector<vector<int>> edgeList2{
        {1,2,6},
        {1,3,5},
        {1,4,5},
        {4,6,-1},
        {4,3,-2},
        {3,2,-2},
        {2,5,-1},
        {5,7,3},
        {6,7,3},
        {3,5,1}
    };  // INT_MAX, 0, 1, 3, 5, 0, 4, 3
    BellManFord(4, edgeList1);
    BellManFord(7, edgeList2);
}