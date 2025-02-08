#pragma once
#include <iostream>
#include <vector>
using namespace std;

// Node structure for AVL Tree
template <typename T>
struct AVLNode {
    T data;               // Data stored in the node
    AVLNode* left;        // Pointer to the left child
    AVLNode* right;       // Pointer to the right child
    int height;           // Height of the node

    AVLNode(T data) : data(data), left(nullptr), right(nullptr), height(1) {}
};

// AVL Tree class
template <typename T>
class AVLTree {
private:
    AVLNode<T>* root;     

    // Inserts a new node with data
    AVLNode<T>* insert(AVLNode<T>* node, T data);         

    // Removes a node with specific data
    AVLNode<T>* remove(AVLNode<T>* node, const T& data);  

    // Finds a node with specific data
    AVLNode<T>* find(AVLNode<T>* node, const T& data) const; 

    // Finds the node with the minimum value in a subtree
    AVLNode<T>* minValueNode(AVLNode<T>* node) const;     

    // Rotate Right and Left
    AVLNode<T>* rotateRight(AVLNode<T>* node);            
    AVLNode<T>* rotateLeft(AVLNode<T>* node);            

    // Return Height of Tree
    int getHeight(AVLNode<T>* node) const;               

    // Calculates the balance factor of a node
    int getBalanceFactor(AVLNode<T>* node) const;      

    // Function for in-order traversal
    void inOrderTraversal(AVLNode<T>* node, vector<T>& items) const; 

    // Delete all nodes in the tree
    void destroy(AVLNode<T>* node);                       

public:
    AVLTree() : root(nullptr) {}  // Constructor
    ~AVLTree();                   // Destructor

    void insert(T data);          
    void remove(const T& data);   
    T* find(const T& data) const; 

    // Display all elements in sorted order
    void displayInOrder() const;  

	// Get all elements in the tree in sorted order
    vector<T> getAllItems() const; 
};


/*
    Gets the height of a given node in the AVL tree

	This functions returns the height of the node, or 0 if the node is null.

    Parameter - node: A pointer to the AVL tree node whose height is to be retrieved
    Return - The height of the node, or 0 if the node is null
*/
template <typename T>
int AVLTree<T>::getHeight(AVLNode<T>* node) const {
    return node ? node->height : 0;
}

/*
    Calculates the balance factor of a node in the AVL tree

	This function gets the balance factor of the given node, which is the difference in heights of its left and right sub trees
	A positive balance factor indicates a left-heavy tree, while a negative balance factor indicates a right-heavy tree.

    Parameter - node: A pointer to the AVL tree node whose balance factor is to be calculated
    Return - The balance factor (difference in heights of left and right subtrees), or 0 if the node is null
*/
template <typename T>
int AVLTree<T>::getBalanceFactor(AVLNode<T>* node) const {
    return node ? getHeight(node->left) - getHeight(node->right) : 0;
}

/*
    Performs a right rotation on the given node in the AVL tree

    This functions rotates the node ( y ) to the right and making its left child ( x ) the new root of the sub tree
	The right sub tree of x is reassigned to y and heights are updated

    Parameter - y: A pointer to the node that needs to be rotated
    Return - A pointer to the new root of the rotated subtree
*/
template <typename T>
AVLNode<T>* AVLTree<T>::rotateRight(AVLNode<T>* y) {
    AVLNode<T>* x = y->left;
    AVLNode<T>* T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

    return x; // Return new root
}

/*
    Performs a left rotation on the given node in the AVL tree

    This function rotates the given node ( x ) to the left, making its right child ( y ) the new root of the subtree. 
    The left subtree of y is reassigned to x, and heights are updated accordingly.

    Parameter - x: A pointer to the node that needs to be rotated
    Return - A pointer to the new root of the rotated subtree
*/
template <typename T>
AVLNode<T>* AVLTree<T>::rotateLeft(AVLNode<T>* x) {
    AVLNode<T>* y = x->right;
    AVLNode<T>* T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update heights
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;

    return y; // Return new root
}

/*
    Performs a left rotation on the given node in the AVL tree

    This function rotates the given node ( x ) to the left, making its right child ( y ) the new root of the subtree.
    The left subtree of y is reassigned to x, and heights are updated accordingly.

    Parameter - x: A pointer to the node that needs to be rotated
    Return - A pointer to the new root of the rotated subtree
*/
template <typename T>
AVLNode<T>* AVLTree<T>::insert(AVLNode<T>* node, T data) {
    if (!node)
        return new AVLNode<T>(data); // Create a new node if the current node is null

    // Insert data into the left or right subtree
    if (data < node->data)
        node->left = insert(node->left, data);
    else if (data > node->data)
        node->right = insert(node->right, data);
    else
        return node; // Duplicate keys are not allowed

    // Update the height of the current node
    node->height = 1 + max(getHeight(node->left), getHeight(node->right));

    // Get the balance factor to check if the node is unbalanced
    int balance = getBalanceFactor(node);

    // Perform rotations if the node is unbalanced
    if (balance > 1 && data < node->left->data) // Left-Left case
        return rotateRight(node);
    if (balance < -1 && data > node->right->data) // Right-Right case
        return rotateLeft(node);
    if (balance > 1 && data > node->left->data) { // Left-Right case
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (balance < -1 && data < node->right->data) { // Right-Left case
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node; // Return the (possibly updated) node
}

/*
    Inserts a new node with the given data into the AVL Tree

    This function inserts the data into the AVL Tree by calling the recursive insert function.
    It ensures the tree remains balanced after insertion.

    Parameter - data: The data to be inserted into the AVL Tree
*/
template <typename T>
void AVLTree<T>::insert(T data) {
    root = insert(root, data);
}

/*
    Finds the node with the minimum value in the subtree rooted at the given node.

    This function traverses the left subtree of the given node to find and return the node with the minimum value

    Parameter - node: A pointer to the root of the subtree to search
    Return - A pointer to the node with the minimum value in the subtree
*/
template <typename T>
AVLNode<T>* AVLTree<T>::minValueNode(AVLNode<T>* node) const {
    AVLNode<T>* current = node;
    while (current && current->left)
        current = current->left;
    return current;
}

/*
    Removes a node with the given data from the AVL Tree and balances the tree if necessary.

    This functions is called recursively and searches and removes the node containing the requested data.
	After removing, it updates the heights of the affected nodes and rotates to maintain balance in the AVL tree.
	It handles this 3 cases : nodes with no children, nodes with one child and nodes with two children (using the inorder successor)

    Parameter - node: A pointer to the root of the subtree to search
    Parameter - data: The data of the node to be removed
    Return - A pointer to the root of the (possibly updated) subtree after removal and balancing
*/
template <typename T>
AVLNode<T>* AVLTree<T>::remove(AVLNode<T>* node, const T& data) {
    if (!node)
        return node; 

    // Search for the node to be deleted
    if (data < node->data)
        node->left = remove(node->left, data);
    else if (data > node->data)
        node->right = remove(node->right, data);
    else {
        // Node with only one child or no child
        if (!node->left || !node->right) {
            AVLNode<T>* temp = node->left ? node->left : node->right;
            if (!temp)
                return nullptr; // No child case
            *node = *temp;      // Copy the contents of the non-empty child
            delete temp;
        }
        else {
            // Node with two children: get the inorder successor (smallest in the right subtree)
            AVLNode<T>* temp = minValueNode(node->right);
            node->data = temp->data; // Copy the inorder successor's data to this node
            node->right = remove(node->right, temp->data); // Delete the inorder successor
        }
    }

    // Update the height of the current node
    node->height = 1 + max(getHeight(node->left), getHeight(node->right));

    // Get the balance factor to check if the node is unbalanced
    int balance = getBalanceFactor(node);

    // Perform rotations if the node is unbalanced
    if (balance > 1 && getBalanceFactor(node->left) >= 0) // Left-Left case
        return rotateRight(node);
    if (balance > 1 && getBalanceFactor(node->left) < 0) { // Left-Right case
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (balance < -1 && getBalanceFactor(node->right) <= 0) // Right-Right case
        return rotateLeft(node);
    if (balance < -1 && getBalanceFactor(node->right) > 0) { // Right-Left case
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node; // Return the (possibly updated) node
}

/*
    Removes a node with the given data from the AVL Tree.

	This function is called doing the removing of the node from the AVL Tree
    It make sures the tree is balanced after removing

    Parameter - data: The data of the node to be removed from the AVL Tree
*/
template <typename T>
void AVLTree<T>::remove(const T& data) {
    root = remove(root, data);
}

/*
    Finds a node with the given data in the AVL Tree.

    This functions recursively searches for the node with the requested data.
	It traveses the tree based on the comparison of the data with the current node's data,searching the left and right subtree accordingly.

    Parameter - node: A pointer to the root of the subtree to search
    Parameter - data: The data to search for in the AVL Tree
    Return - A pointer to the node containing the data, or nullptr if the data is not found
*/
template <typename T>
AVLNode<T>* AVLTree<T>::find(AVLNode<T>* node, const T& data) const {
    if (!node || node->data == data)
        return node; // Base case: node found or not found
    if (data < node->data)
        return find(node->left, data); // Search in the left subtree
    return find(node->right, data);    // Search in the right subtree
}

/*
    Finds data in the AVL Tree and returns a pointer to it.

    This function searches for the given data in the Tree by calling a recursive find function.
	If the data is found, it returns a pointer to the data else it returns a nullptr

    Parameter - data: The data to search for in the AVL Tree
    Return - A pointer to the data if found, otherwise nullptr
*/
template <typename T>
T* AVLTree<T>::find(const T& data) const {
    AVLNode<T>* foundNode = find(root, data);
    return foundNode ? &(foundNode->data) : nullptr;
}

/*
    Performs in-order traversal of the AVL Tree and stores the elements in a vector.

    This function recursively traverses the AVL Tree in in-order (left, root, right) manner and stores the elements of the tree in a vector. 
    The vector will contain the elements in ascending order.

    Parameter - node: A pointer to the root of the subtree to traverse
    Parameter - items: A reference to a vector to store the elements of the tree
*/
template <typename T>
void AVLTree<T>::inOrderTraversal(AVLNode<T>* node, vector<T>& items) const {
    if (node) {
        inOrderTraversal(node->left, items);  // Traverse left subtree
        items.push_back(node->data);          // Visit node
        inOrderTraversal(node->right, items); // Traverse right subtree
    }
}

/*
    Returns all elements in the AVL Tree in sorted order.

    This function performs an in-order traversal of the AVL Tree and collects all elements into a vector. 
    The elements are returned in ascending order.

    Return - A vector containing all elements of the AVL Tree in sorted order
*/
template <typename T>
vector<T> AVLTree<T>::getAllItems() const {
    vector<T> items;
    inOrderTraversal(root, items);
    return items;
}

/*
    Recursively deletes all nodes in the AVL Tree.

	This function is called recursively to delete all nodes in the AVL Tree.

    Parameter - node: A pointer to the root of the subtree to delete
*/
template <typename T>
void AVLTree<T>::destroy(AVLNode<T>* node) {
    if (node) {
        destroy(node->left);  // Delete left subtree
        destroy(node->right); // Delete right subtree
        delete node;          // Delete current node
    }
}

// Destructor for the AVL Tree.
template <typename T>
AVLTree<T>::~AVLTree() {
    destroy(root);
}

/*
    Displays all elements in the AVL Tree in sorted order.

    This function retrieves all elements in the AVL Tree using `getAllItems` and prints them in ascending order.

    Output - Prints each element of the AVL Tree to the standard output, one per line
*/
template <typename T>
void AVLTree<T>::displayInOrder() const {
    vector<T> items = getAllItems();
    for (const auto& item : items) {
        cout << item << endl;
    }
}