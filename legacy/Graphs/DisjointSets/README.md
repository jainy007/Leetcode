Summary of the “disjoint set” data structure
--------------------------------------------
https://leetcode.com/explore/learn/card/graph/618/disjoint-set/3844/


The main idea of a “disjoint set” is to have all connected vertices have the same parent node or root node, whether directly or indirectly connected. To check if two vertices are connected, we only need to check if they have the same root node.

The two most important functions for the “disjoint set” data structure are the find function and the union function. The find function locates the root node of a given vertex. The union function connects two previously unconnected vertices by giving them the same root node. There is another important function named connected, which checks the “connectivity” of two vertices. The find and union functions are essential for any question that uses the “disjoint set” data structure.

Disjoint set has following functions:

1) find Function (Basic and path optimized)
2) union Function (basic and optimized by rank)
3) connected function

Depth First Search

1) Traverse all NODES of the graph using "recursive STACK traversal RST" 
    (Store nodes in stack; like A, B ...X)
2) Traverse all PATHS between 2 vertices of the graph using RST
    (Store paths in stack; like [(A), (A, B), (A, B, D) ...(X, Y, ... Z)])

Breadth First Search

1) Traverse all NODES of the graph using "recursive QUEUE traversal" RQT
    (Store nodes in queue; like A, B ...X)
2) Find Shortest Path between 2 vertices of the graph






