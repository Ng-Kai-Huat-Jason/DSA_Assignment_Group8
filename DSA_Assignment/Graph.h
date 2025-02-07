#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include <iostream>
using namespace std;

// Graph class template
// Uses an adjacency matrix (a vector of vectors of integers) and a vector of nodes that are strings representing either an actor’s name or a movie title.
// The graph is undirected so an edge exists between two nodes if there is an association like when an actor stars in a movie,
// so an edge is created between the actor’s name and the movie title.
template <typename T>
class Graph {
private:
    int count;
    vector<vector<int>> adjacencyMatrix; // Adjacency matrix using a vector of vectors
    vector<T> nodes;                     // Mapping indices to nodes

public:
    Graph();

    bool nodeExists(const T& node);
    int getNodeIndex(const T& node);
    void addNode(const T& node);
    void addEdge(const T& source, const T& destination);
    vector<T> getNeighbors(const T& node);
    void removeNode(const T& node);
    void displayMatrix() const;
    void bfs(const T& startNode);

    // New method: update a node's value (for example, when an actor's name changes)
    void updateNode(const T& oldNode, const T& newNode);

    // New method: List movies for an actor (assumes actor is stored as string)
    vector<T> listMoviesForActor(const T& actorName);

    // New method: List actors for a movie
    vector<T> listActorsForMovie(const T& movieTitle);

    // New method: Get known actors via two-level connections for an actor
    vector<T> getKnownActors(const T& actorName);

    const vector<T>& getNodes() const;
};

#include "Graph.cpp" // Include the implementation file

#endif // GRAPH_H
