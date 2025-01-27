#include "Graph.h"

// Check if a node exists in the graph
template <typename T>
bool Graph<T>::nodeExists(const T& node) {
    return getNodeIndex(node) != -1; // Returns true if the node exists (index is not -1)
}

// Get the index of a node in the nodes vector
template <typename T>
int Graph<T>::getNodeIndex(const T& node) {
    auto it = find(nodes.begin(), nodes.end(), node);
    if (it != nodes.end()) {
        auto index = distance(nodes.begin(), it);
        return static_cast<int>(index);
    }
    return -1; // Node not found
}


// Add a new node to the graph
template <typename T>
void Graph<T>::addNode(const T& node) {
    if (getNodeIndex(node) == -1) { // Avoid duplicates
        nodes.push_back(node);
        count++;

        // Resize the adjacency matrix to accommodate the new node
        for (auto& row : adjacencyMatrix) {
            row.push_back(0); // Add a column for the new node
        }
        adjacencyMatrix.push_back(vector<int>(count, 0)); // Add a row for the new node

    }
}


// Add an edge to the graph
template <typename T>
void Graph<T>::addEdge(const T& source, const T& destination) {
    int srcIndex = getNodeIndex(source);
    int destIndex = getNodeIndex(destination);

    if (srcIndex == -1 || destIndex == -1) {
        cout << "Error: One or both nodes not found in the graph!" << endl;
        return;
    }

    // Ensure the adjacency matrix is large enough
    if (srcIndex >= adjacencyMatrix.size()) {
        adjacencyMatrix.resize(srcIndex + 1, vector<int>(adjacencyMatrix.size(), 0));
    }
    if (destIndex >= adjacencyMatrix.size()) {
        adjacencyMatrix.resize(destIndex + 1, vector<int>(adjacencyMatrix.size(), 0));
    }

    adjacencyMatrix[srcIndex][destIndex] = 1;
}


// Get neighbors of a node
template <typename T>
vector<T> Graph<T>::getNeighbors(const T& node) {
    int nodeIndex = getNodeIndex(node);
    vector<T> neighbors;

    if (nodeIndex == -1) {
        return neighbors;
    }

    for (int i = 0; i < nodes.size(); ++i) {  // Use nodes.size() instead of count
        if (adjacencyMatrix[nodeIndex][i] == 1) {
            neighbors.push_back(nodes[i]);
        }
    }

    return neighbors;
}


// Print the graph
template <typename T>
void Graph<T>::displayMatrix() const {
    for (const auto& row : adjacencyMatrix) {
        for (int value : row) {
            cout << value << " ";
        }
        cout << endl;
    }
}

// Explicit template instantiation
template class Graph<string>;
