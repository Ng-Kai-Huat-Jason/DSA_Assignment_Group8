#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <iostream>
using namespace std;

template <typename T>
class Graph {
private:
    int count;
    vector<vector<int>> adjacencyMatrix; // Adjacency matrix using a vector of vectors
    vector<T> nodes;            // Mapping indices to nodes

    int getNodeIndex(const T& node);

public:
    Graph() {}

    void addNode(const T& node);
    void addEdge (const T& source, const T& destination);
    vector<T> getNeighbors(const T& node);
    void displayMatrix() const;
};

#endif // GRAPH_H
