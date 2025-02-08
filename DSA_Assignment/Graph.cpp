#include "Graph.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

// -------------------------
// QuickSort Helper Functions
// -------------------------

// Swaps two elements.
template <typename T>
void swapElements(T& a, T& b) {
    T temp = a;
    a = b;
    b = temp;
}

// Partition function for quicksort (ascending order).
template <typename T>
int partition(vector<T>& arr, int low, int high) {
    T pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            i++;
            swapElements(arr[i], arr[j]);
        }
    }
    swapElements(arr[i + 1], arr[high]);
    return i + 1;
}

// QuickSort implementation.
template <typename T>
void quickSort(vector<T>& arr, int low, int high) {
    if (low < high) {
        int p = partition(arr, low, high);
        quickSort(arr, low, p - 1);
        quickSort(arr, p + 1, high);
    }
}

// Graph Implementations

// Constructor
template <typename T>
Graph<T>::Graph() : count(0) {
    // Initially, the graph is empty.
}

// Check if a node exists in the graph.
template <typename T>
bool Graph<T>::nodeExists(const T& node) {
    return getNodeIndex(node) != -1;
}

// Get the index of a node in the nodes vector.
template <typename T>
int Graph<T>::getNodeIndex(const T& node) {
    for (int i = 0; i < static_cast<int>(nodes.size()); i++) {
        if (nodes[i] == node)
            return i;
    }
    return -1; // Node not found.
}

// Add a new node to the graph.
template <typename T>
void Graph<T>::addNode(const T& node) {
    if (getNodeIndex(node) == -1) { // Avoid duplicates.
        nodes.push_back(node);
        count++;

        // Resize the adjacency matrix: add one column to each existing row.
        for (size_t i = 0; i < adjacencyMatrix.size(); i++) {
            adjacencyMatrix[i].push_back(0);
        }
        // Add a new row for the new node.
        adjacencyMatrix.push_back(vector<int>(count, 0));
    }
}

// Remove a node from the graph.
template <typename T>
void Graph<T>::removeNode(const T& node) {
    int nodeIndex = getNodeIndex(node);
    if (nodeIndex == -1)
        return; // Node not found.

    // Remove node from the nodes vector.
    nodes.erase(nodes.begin() + nodeIndex);

    // Remove the corresponding row from the adjacency matrix.
    adjacencyMatrix.erase(adjacencyMatrix.begin() + nodeIndex);
    // Remove the corresponding column from each remaining row.
    for (size_t i = 0; i < adjacencyMatrix.size(); i++) {
        adjacencyMatrix[i].erase(adjacencyMatrix[i].begin() + nodeIndex);
    }
}

// Add an edge to the graph (undirected).
template <typename T>
void Graph<T>::addEdge(const T& source, const T& destination) {
    int srcIndex = getNodeIndex(source);
    int destIndex = getNodeIndex(destination);

    if (srcIndex == -1 || destIndex == -1) {
        cout << "[Error] One or both nodes not found in the graph: \""
            << source << "\", \"" << destination << "\"\n";
        return;
    }
    adjacencyMatrix[srcIndex][destIndex] = 1;
    adjacencyMatrix[destIndex][srcIndex] = 1;
}

// Get neighbors of a node.
template <typename T>
vector<T> Graph<T>::getNeighbors(const T& node) {
    int nodeIndex = getNodeIndex(node);
    vector<T> neighbors;
    if (nodeIndex == -1)
        return neighbors;
    for (int i = 0; i < static_cast<int>(nodes.size()); ++i) {
        if (adjacencyMatrix[nodeIndex][i] == 1) {
            neighbors.push_back(nodes[i]);
        }
    }
    return neighbors;
}

// Display the adjacency matrix.
template <typename T>
void Graph<T>::displayMatrix() const {
    for (size_t i = 0; i < adjacencyMatrix.size(); i++) {
        for (size_t j = 0; j < adjacencyMatrix[i].size(); j++) {
            cout << adjacencyMatrix[i][j] << " ";
        }
        cout << endl;
    }
}

// Breadth-first search (BFS) starting from a given node.
template <typename T>
void Graph<T>::bfs(const T& startNode) {
    int startIndex = getNodeIndex(startNode);
    if (startIndex == -1) {
        cout << "Actor \"" << startNode << "\" does not exist in the graph." << endl;
        return;
    }

    // Initialize visited array and parent tracking.
    vector<bool> visited(nodes.size(), false);
    vector<int> parent(nodes.size(), -1);
    vector<int> toVisit;
    toVisit.push_back(startIndex);
    visited[startIndex] = true;

    // Display the actor's movies and co-actors.
    cout << startNode << " starred in:\n";
    vector<T> movies = getNeighbors(startNode);
    for (size_t i = 0; i < movies.size(); i++) {
        cout << "\n - " << movies[i] << ":\n";
        vector<T> coActors = getNeighbors(movies[i]);
        for (size_t j = 0; j < coActors.size(); j++) {
            if (coActors[j] != startNode)
                cout << "    * " << coActors[j] << "\n";
        }
    }

    // Display the related actors , co-actors (second-level connections).
    cout << "\n" << startNode << " knows the following actors:\n";
    for (size_t i = 0; i < movies.size(); i++) {
        vector<T> coActors = getNeighbors(movies[i]);
        for (size_t j = 0; j < coActors.size(); j++) {
            if (coActors[j] != startNode) {
                cout << "\n - " << coActors[j] << " starred in:\n";
                vector<T> coActorMovies = getNeighbors(coActors[j]);
                for (size_t k = 0; k < coActorMovies.size(); k++) {
                    vector<T> secondLevelCoActors = getNeighbors(coActorMovies[k]);
                    cout << "    * In movie \"" << coActorMovies[k] << "\":\n";
                    for (size_t l = 0; l < secondLevelCoActors.size(); l++) {
                        if (secondLevelCoActors[l] != coActors[j] && secondLevelCoActors[l] != startNode)
                            cout << "      - " << secondLevelCoActors[l] << "\n";
                    }
                }
            }
        }
    }
    cout << endl;
}

// Update a node in the graph.
template <typename T>
void Graph<T>::updateNode(const T& oldNode, const T& newNode) {
    int idx = getNodeIndex(oldNode);
    if (idx == -1) {
        cout << "[Warning] Node \"" << oldNode
            << "\" not found in the graph. Adding new node \""
            << newNode << "\" instead.\n";
        addNode(newNode);
        return;
    }
    vector<T> neighbors = getNeighbors(oldNode);
    removeNode(oldNode);
    addNode(newNode);
    for (size_t i = 0; i < neighbors.size(); i++) {
        if (neighbors[i] != newNode)
            addEdge(newNode, neighbors[i]);
    }
}

// List movies for an actor (assumes actor is stored as string).
template <typename T>
vector<T> Graph<T>::listMoviesForActor(const T& actorName) {
    vector<T> movies = getNeighbors(actorName);
    if (!movies.empty()) {
        quickSort(movies, 0, static_cast<int>(movies.size()) - 1);
    }
    return movies;
}

// List actors for a movie (assumes movie is stored as string).
template <typename T>
vector<T> Graph<T>::listActorsForMovie(const T& movieTitle) {
    vector<T> actors = getNeighbors(movieTitle);
    if (!actors.empty()) {
        quickSort(actors, 0, static_cast<int>(actors.size()) - 1);
    }
    return actors;
}

// Return all nodes in the graph.
template <typename T>
const vector<T>& Graph<T>::getNodes() const {
    return nodes;
}

// Explicit template instantiation for type string.
template class Graph<string>;
