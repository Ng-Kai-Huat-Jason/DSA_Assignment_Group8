#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include <iostream>
using namespace std;

// Graph Template Class: Represents an undirected graph using an adjacency matrix.
// In our use-case, nodes represent actor names or movie titles.
template <typename T>
class Graph {
private:
    int count;                          ///< Number of nodes in the graph.
    vector<vector<int>> adjacencyMatrix;///< Adjacency matrix representation.
    vector<T> nodes;                    ///< Node values.

public:
    // Default constructor.
    Graph();

    // Check if a node exists in the graph.
    bool nodeExists(const T& node);

    // Get the index of a node in the graph.
    int getNodeIndex(const T& node);

    // Add a node to the graph.
    void addNode(const T& node);

    // Add an edge between two nodes.
    void addEdge(const T& source, const T& destination);

    // Get all nodes adjacent to a given node.
    std::vector<T> getNeighbors(const T& node);

    // Remove a node and its associated edges from the graph.
    void removeNode(const T& node);

    // Display the adjacency matrix.
    void displayMatrix() const;

    // Perform breadth-first search starting from a given node.
    void bfs(const T& startNode);

    // Update a node in the graph.
    void updateNode(const T& oldNode, const T& newNode);

    // Get all movies for a specific actor.
    std::vector<T> listMoviesForActor(const T& actorName);

    // Get all actors for a specific movie.
    std::vector<T> listActorsForMovie(const T& movieTitle);

    // Return all nodes in the graph.
    const std::vector<T>& getNodes() const;
};

#include "Graph.cpp"

#endif // GRAPH_H
