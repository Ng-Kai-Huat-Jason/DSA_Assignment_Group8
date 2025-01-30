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

// Remove a node from the graph
template <typename T>
void Graph<T>::removeNode(const T& node) {
    int nodeIndex = getNodeIndex(node);
    if (nodeIndex == -1) return; // Node not found

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
    adjacencyMatrix[destIndex][srcIndex] = 1;
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

// BFS
template <typename T>
void Graph<T>::bfs(const T& startNode) {
    int startIndex = getNodeIndex(startNode);
    if (startIndex == -1) {
        cout << "Actor \"" << startNode << "\" does not exist in the graph." << endl;
        return;
    }

    // Initialize visited array and parent tracking
    vector<bool> visited(nodes.size(), false);
    vector<int> parent(nodes.size(), -1); // Tracks the parent node index
    vector<int> toVisit;                 // List to process nodes (acts like a queue)
    toVisit.push_back(startIndex);
    visited[startIndex] = true;

    //Display the actor's movies and co-actors
    cout << startNode << " starred in:\n";
    vector<T> movies = getNeighbors(startNode);
    for (const T& movie : movies) {
        cout << " \n - " << movie << ":\n";
        vector<T> coActors = getNeighbors(movie);
        for (const T& coActor : coActors) {
            if (coActor != startNode) {
                cout << "    * " << coActor << "\n";
            }
        }
    }

    //Display the co-actors' co-actors (second-level connections)
    cout << "\n" << startNode << " knows the following actors:\n";
    for (const T& movie : movies) {
        vector<T> coActors = getNeighbors(movie);
        for (const T& coActor : coActors) {
            if (coActor != startNode) {
                cout << " \n - " << coActor << " starred in:\n";
                vector<T> coActorMovies = getNeighbors(coActor);
                for (const T& coActorMovie : coActorMovies) {
                    vector<T> secondLevelCoActors = getNeighbors(coActorMovie);
                    cout << "    * In movie \"" << coActorMovie << "\":\n";
                    for (const T& secondLevelCoActor : secondLevelCoActors) {
                        if (secondLevelCoActor != coActor && secondLevelCoActor != startNode) {
                            cout << "      - " << secondLevelCoActor << "\n";
                        }
                    }
                }
            }
        }
    }
    cout << endl;
}

// Explicit template instantiation
template class Graph<string>;
