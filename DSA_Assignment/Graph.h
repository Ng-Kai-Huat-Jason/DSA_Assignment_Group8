#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <iostream>
using namespace std;

template <typename T>
class Graph {
private:
    vector<vector<int>> adjList; // Adjacency list using a vector of vectors
    vector<T> nodes;            // Mapping indices to nodes

    int getNodeIndex(const T& node);

public:
    Graph() {}

    void addNode(const T& node);
    void addEdge(const T& source, const T& destination);
    vector<T> getNeighbors(const T& node);
    void printGraph() const;
};

#endif // GRAPH_H
