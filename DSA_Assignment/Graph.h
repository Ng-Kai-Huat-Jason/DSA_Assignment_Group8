#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include <iostream>
using namespace std;

 // Graph Template Class , this is use to represent cast for my relationship between actors and movies
// Implements a adjacency matrix representation of a graph
// The graph is undirected so if a actor is in a movie then there is a edge between the actor and the movie
template <typename T>
class Graph {
private:
    int count;  ///< Keeps track of the number of nodes in the graph.
    vector<vector<int>> adjacencyMatrix; ///< Adjacency matrix representation.
    vector<T> nodes; ///< Stores node values (actor names, movie titles, etc.).

public:

    // Default constructor 
    Graph();

	// Function to check if a node exists in the graph
    bool nodeExists(const T& node);

	 // Function to get the index of a node in the graph
    int getNodeIndex(const T& node);

	 // Add a node to the graph
    void addNode(const T& node);

	 // Add an edge between two nodes 
    void addEdge(const T& source, const T& destination);


    // Functions to get all nodes connected to the given node 
    vector<T> getNeighbors(const T& node);

	// Function to remove a node from the graph and its edges
    void removeNode(const T& node);

	 // Function to display adjacent matrix of the graph
    void displayMatrix() const;

    // Function to do Breath First Search from a given node
    void bfs(const T& startNode);

	 // Function to update a node in the graph
    void updateNode(const T& oldNode, const T& newNode);

	 // Function to get all movies for a specific actor
    vector<T> listMoviesForActor(const T& actorName);

    // Function to get all actors for a specific movie
    vector<T> listActorsForMovie(const T& movieTitle);

	// Return all nodes in the graph
    const vector<T>& getNodes() const;
};

#include "Graph.cpp" 

#endif // GRAPH_H
