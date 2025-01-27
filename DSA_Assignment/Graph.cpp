#include "Graph.h"

// Get the index of a node in the nodes vector
template <typename T>
int Graph<T>::getNodeIndex(const T& node) {
    for (int i = 0; i < nodes.size(); ++i) {
        if (nodes[i] == node) {
            return i;
        }
    }
    return -1; // Node not found
}

// Add a new node to the graph
template <typename T>
void Graph<T>::addNode(const T& node) {
    if (getNodeIndex(node) == -1) { // Avoid duplicates
        nodes.push_back(node);
        adjList.push_back({});
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

    adjList[srcIndex].push_back(destIndex);
    adjList[destIndex].push_back(srcIndex); // For undirected graph
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
    for (int neighborIndex : adjList[nodeIndex]) {
        neighbors.push_back(nodes[neighborIndex]);
    }
    return neighbors;
}

// Print the graph
template <typename T>
void Graph<T>::printGraph() const {
    for (int i = 0; i < nodes.size(); ++i) {
        cout << nodes[i] << ": ";
        for (int neighborIndex : adjList[i]) {
            cout << nodes[neighborIndex] << " ";
        }
        cout << endl;
    }
}

// Explicit template instantiation
template class Graph<string>;
