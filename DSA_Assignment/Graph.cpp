#include "Graph.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

// -------------------------
// QuickSort Helper Functions
// -------------------------

/*
    Swaps the values of two elements

    This function swaps the values of two given elements using a temporary variable

    Parameter - a: The first element to swap
    Parameter - b: The second element to swap
    Return - None (modifies the input elements in place)
*/
template <typename T>
void swapElements(T& a, T& b) {
    T temp = a;
    a = b;
    b = temp;
}

/*
    Partitions a vector for Quick Sort

	This functions selects a pivot point and rearranges the elements in the vector so that
	all elements less than the pivot are placed to the left of the pivot and those larger or equal to the pivot are placed to the right

    Parameter - arr: The vector to be partitioned
    Parameter - low: The starting index of the partition range
    Parameter - high: The ending index of the partition range
    Return - The index of the pivot element after partitioning
*/
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

/*
    Sorts a vector using Quick Sort

    This function recursively applies the Quick Sort algorithm to sort a given vector.
    It partitions the vector around a pivot element, then recursively sorts the
    left and right subarrays.

    Parameter - arr: The vector to be sorted
    Parameter - low: The starting index of the sorting range
    Parameter - high: The ending index of the sorting range
    Return - None (modifies the vector in place)
*/
template <typename T>
void quickSort(vector<T>& arr, int low, int high) {
    if (low < high) {
        int p = partition(arr, low, high);
        quickSort(arr, low, p - 1);
        quickSort(arr, p + 1, high);
    }
}



//////////////// Graph Implementation ////////////////

// Constructor
template <typename T>
Graph<T>::Graph() : count(0) {
}

/*
    Checks if a node exists in the graph

    This function determines whether a given node exists in the graph by checking if its index is valid

    Parameter - node: The node to check
    Return - True if the node exists, otherwise false
*/
template <typename T>
bool Graph<T>::nodeExists(const T& node) {
    return getNodeIndex(node) != -1;
}

/*
    Retrieves the index of a node in the graph

    This function searches for a given node in the graph's node list and returns its index. If the node is not found, it returns -1

    Parameter - node: The node whose index is to be found
    Return - The index of the node if found, otherwise -1
*/
template <typename T>
int Graph<T>::getNodeIndex(const T& node) {
    for (int i = 0; i < static_cast<int>(nodes.size()); i++) {
        if (nodes[i] == node)
            return i;
    }
    return -1; // Node not found.
}

/*
    Adds a new node to the graph

	This function adds a new node to the graph if it does not already exist
	It also updates the adjacency matrix by adding a new row and a new column to accommodate the new node

    Parameter - node: The node to be added to the graph
    Return - None (modifies the graph structure)
*/
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

/*
    Removes a node from the graph

    This function removes a node from the graph by deleting it from the nodes list
    and updating the adjacency matrix to remove all associated edges.

    Parameter - node: The node to be removed from the graph
    Return - None (modifies the graph structure)
*/
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

/*
	Adds an edge between two nodes in the graph - used for Cast Relationships

    This function creates an undirected edge between two nodes by updating 
    the adjacency matrix. If either node does not exist, an error message is displayed.

    Parameter - source: The starting node of the edge
    Parameter - destination: The ending node of the edge
    Return - None (modifies the adjacency matrix)
*/
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

/*
    Retrieves the neighbors of a given node

    This function returns a list of all nodes that are directly connected to the given node in the graph.

    Parameter - node: The node whose neighbors are to be found
    Return - A vector containing the neighboring nodes
*/
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

/*
    Displays the adjacency matrix of the graph

    This function prints the adjacency matrix, showing the connections between nodes in the graph. 
    A value of 1 means an edge exists between two nodes while 0 means no connection.

    Parameter - None
    Return - None (outputs the adjacency matrix to the console)
*/
template <typename T>
void Graph<T>::displayMatrix() const {
    for (size_t i = 0; i < adjacencyMatrix.size(); i++) {
        for (size_t j = 0; j < adjacencyMatrix[i].size(); j++) {
            cout << adjacencyMatrix[i][j] << " ";
        }
        cout << endl;
    }
}

/*
    Performs a Breadth-First Search (BFS) traversal in the graph

    This function starts from the given node and explores its neighbors,
    displaying all movies the actor has starred in along with their co-actors.
    It also shows second-level connections (actors that worked with the co-actors)

    This is used to get actors that know actors indirectly or directly

    Parameter - startNode: The node (actor) from which BFS traversal starts
    Return - None (outputs the traversal results to the console)
*/
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

/*
    Updates a node in the graph

    This function replaces an existing node with a new node while preserving its connections.
    If the old node is not found, the new node is added instead

    Parameter - oldNode: The node to be replaced
    Parameter - newNode: The node to replace the old one
    Return - None (modifies the graph structure)
*/
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

/*
    Retrieves and sorts the list of movies an actor has starred in

    This function fetches all movies associated with the given actor from the graph
    and sorts them in ascending order using Quick Sort

    Parameter - actorName: The name of the actor whose movies are to be listed
    Return - A sorted vector containing the movies the actor has starred in
*/
template <typename T>
vector<T> Graph<T>::listMoviesForActor(const T& actorName) {
    vector<T> movies = getNeighbors(actorName);
    if (!movies.empty()) {
        quickSort(movies, 0, static_cast<int>(movies.size()) - 1);
    }
    return movies;
}

/*
    Retrieves and sorts the list of actors in a given movie

    This function fetches all actors associated with the given movie from the graph
    and sorts them in ascending order using Quick Sort

    Parameter - movieTitle: The title of the movie whose actors are to be listed
    Return - A sorted vector containing the actors in the movie
*/
template <typename T>
vector<T> Graph<T>::listActorsForMovie(const T& movieTitle) {
    vector<T> actors = getNeighbors(movieTitle);
    if (!actors.empty()) {
        quickSort(actors, 0, static_cast<int>(actors.size()) - 1);
    }
    return actors;
}

/*
    Retrieves all nodes in the graph

    This function returns a constant reference to the vector containing
    all nodes in the graph.

    Parameter - None
    Return - A constant reference to the vector of nodes
*/
template <typename T>
const vector<T>& Graph<T>::getNodes() const {
    return nodes;
}

// Explicit template instantiation for type string.
template class Graph<string>;
