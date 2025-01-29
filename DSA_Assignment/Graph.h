#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <iostream>
#include "Actor.h"
#include "Movie.h"

using namespace std;

template <typename T>
class Graph {
private:
    int count;
    vector<vector<int>> adjacencyMatrix; // Adjacency matrix using a vector of vectors
    vector<T> nodes;            // Mapping indices to nodes

public:
    Graph() {}
    bool nodeExists(const T& node);
    int getNodeIndex(const T& node);
    void addNode(const T& node);
    void addEdge (const T& source, const T& destination);
    vector<T> getNeighbors(const T& node);
    void displayMatrix() const;
    void bfs(const T& startNode);
};

#endif // GRAPH_H
