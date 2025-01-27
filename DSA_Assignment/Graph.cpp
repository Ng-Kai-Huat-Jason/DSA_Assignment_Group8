#include "Graph.h"

// Get the index of a node in the nodes vector
template <typename T>
int Graph<T>::getNodeIndex(const T& node) {
    auto it = find(nodes.begin(), nodes.end(), node);
    if (it != nodes.end()) {
        return distance(nodes.begin(), it);
    }
    return -1; // Node not found
}

// Add a new node to the graph
template <typename T>
void Graph<T>::addNode(const T& node) {
    if (getNodeIndex(node) == -1) { // Avoid duplicates
        nodes.push_back(node);
        adjacencyMatrix.push_back({});
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

    adjacencyMatrix[srcIndex][destIndex] == 1;
    cout << "Linked actor \"" << source << "\" to movie \"" destination << "\"\n";
}

// Get neighbors of a node
template <typename T>
vector<T> Graph<T>::getNeighbors(const T& node) {
    int nodeIndex = getNodeIndex(node);
    if (nodeIndex == -1) {
        cout << "Error: Node not found!" << endl;
        return {};
    }

    vector<T> neighbors;
    for (int i = 0; i < count; ++i) {
        if (adjacencyMatrix[1][nodeIndex] == 1) {
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
