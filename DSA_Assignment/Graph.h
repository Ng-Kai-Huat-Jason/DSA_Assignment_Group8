
// Graph class template
// Uses an adjacency matrix (a vector of vectors of integers) and a vector of nodes that are strings representing either an actor’s name or a movie title
// The graph is undirected so an edge exists between two nodes 
// if there is an association like when an actor stars in a movie, 
// so an edge is created between the actor’s name and the movie title

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
    void removeNode(const T& node);

    const vector<T>& getNodes() const {
        return nodes;
    }
};

#include "Graph.tpp"
#endif // GRAPH_H
