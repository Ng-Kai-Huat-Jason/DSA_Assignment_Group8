#include "Graph.h"
#include <algorithm>

// Constructor
template <typename T>
Graph<T>::Graph() : count(0) {
    // Initially, the graph is empty
}

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
        return static_cast<int>(distance(nodes.begin(), it));
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

// Remove a node from the graph
template <typename T>
void Graph<T>::removeNode(const T& node) {
    int nodeIndex = getNodeIndex(node);
    if (nodeIndex == -1)
        return; // Node not found

    // Remove from nodes vector
    nodes.erase(nodes.begin() + nodeIndex);

    // Remove from adjacency matrix
    adjacencyMatrix.erase(adjacencyMatrix.begin() + nodeIndex);
    for (auto& row : adjacencyMatrix) {
        row.erase(row.begin() + nodeIndex);
    }
}

// Add an edge to the graph
template <typename T>
void Graph<T>::addEdge(const T& source, const T& destination) {
    int srcIndex = getNodeIndex(source);
    int destIndex = getNodeIndex(destination);

    if (srcIndex == -1 || destIndex == -1) {
        cout << "[Error] One or both nodes not found in the graph: \"" << source << "\", \"" << destination << "\"\n";
        return;
    }
    adjacencyMatrix[srcIndex][destIndex] = 1;
    adjacencyMatrix[destIndex][srcIndex] = 1;
}

// Get neighbors of a node
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

// Display the adjacency matrix of the graph
template <typename T>
void Graph<T>::displayMatrix() const {
    for (const auto& row : adjacencyMatrix) {
        for (int value : row) {
            cout << value << " ";
        }
        cout << endl;
    }
}

// Breadth-first search (BFS) starting from a given node
template <typename T>
void Graph<T>::bfs(const T& startNode) {
    int startIndex = getNodeIndex(startNode);
    if (startIndex == -1) {
        cout << "[Error] Node \"" << startNode << "\" does not exist in the graph." << endl;
        return;
    }

    // Initialize visited array and queue for BFS
    vector<bool> visited(nodes.size(), false);
    vector<int> toVisit;
    toVisit.push_back(startIndex);
    visited[startIndex] = true;

    // Display the actor's movies and co-actors
    cout << startNode << " starred in:\n";
    vector<T> movies = getNeighbors(startNode);
    for (const T& movie : movies) {
        cout << " - " << movie << ":\n";
        vector<T> coActors = getNeighbors(movie);
        for (const T& coActor : coActors) {
            if (coActor != startNode)
                cout << "    * " << coActor << "\n";
        }
    }
    cout << endl;
}

// New method: update a node's value (for example, when an actor's name changes)
template <typename T>
void Graph<T>::updateNode(const T& oldNode, const T& newNode) {
    int idx = getNodeIndex(oldNode);
    if (idx == -1) {
        cout << "[Error] Node \"" << oldNode << "\" not found for update.\n";
        return;
    }
    // Save the current neighbor list
    vector<T> neighbors = getNeighbors(oldNode);
    // Remove the old node completely
    removeNode(oldNode);
    // Add the new node
    addNode(newNode);
    // Reconnect all neighbors to the new node
    for (const T& neighbor : neighbors) {
        if (neighbor != newNode) { // Avoid duplicate edge
            addEdge(newNode, neighbor);
        }
    }
}

// New method: List movies for an actor (assumes actor is stored as string)
template <typename T>
vector<T> Graph<T>::listMoviesForActor(const T& actorName) {
    vector<T> movies = getNeighbors(actorName);
    // Sort alphabetically
    sort(movies.begin(), movies.end());
    return movies;
}

// New method: List actors for a movie
template <typename T>
vector<T> Graph<T>::listActorsForMovie(const T& movieTitle) {
    vector<T> actors = getNeighbors(movieTitle);
    // Sort alphabetically
    sort(actors.begin(), actors.end());
    return actors;
}

// New method: Get known actors via two-level connections for an actor
template <typename T>
vector<T> Graph<T>::getKnownActors(const T& actorName) {
    vector<T> level1 = getNeighbors(actorName);
    vector<T> level2;
    for (const T& movie : level1) {
        vector<T> coActors = getNeighbors(movie);
        for (const T& coActor : coActors) {
            if (coActor != actorName)
                level2.push_back(coActor);
        }
    }
    // Combine level1 and level2
    vector<T> known = level1;
    known.insert(known.end(), level2.begin(), level2.end());
    // Remove duplicates by sorting and using unique
    sort(known.begin(), known.end());
    known.erase(unique(known.begin(), known.end()), known.end());
    return known;
}

// Get all nodes in the graph (const reference)
template <typename T>
const vector<T>& Graph<T>::getNodes() const {
    return nodes;
}

// Explicit template instantiation for type string
template class Graph<string>;
