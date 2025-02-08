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


// Returns the height of a node. If the node is null, returns 0.
template <typename T>
int AVLTree<T>::getHeight(AVLNode<T>* node) const {
    return node ? node->height : 0;
}

// Calculates the balance factor of a node (difference in heights of left and right subtrees).
template <typename T>
int AVLTree<T>::getBalanceFactor(AVLNode<T>* node) const {
    return node ? getHeight(node->left) - getHeight(node->right) : 0;
}

// Performs a right rotation on the given node (y) and returns the new root of the subtree.
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

// Performs a left rotation on the given node (x) and returns the new root of the subtree.
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

// Inserts a new node with the given data into the AVL Tree and balances the tree if necessary.
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

// Insert to AVL Tree
template <typename T>
void AVLTree<T>::insert(T data) {
    root = insert(root, data);
}

// Finds the node with the minimum value in the subtree rooted at the given node.
template <typename T>
AVLNode<T>* AVLTree<T>::minValueNode(AVLNode<T>* node) const {
    AVLNode<T>* current = node;
    while (current && current->left)
        current = current->left;
    return current;
}

// Removes a node with the given data from the AVL Tree and balances the tree if necessary.
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

// Remove Data from AVL Tree
template <typename T>
void AVLTree<T>::remove(const T& data) {
    root = remove(root, data);
}

// Finds a node with the given data in the AVL Tree.
template <typename T>
AVLNode<T>* AVLTree<T>::find(AVLNode<T>* node, const T& data) const {
    if (!node || node->data == data)
        return node; // Base case: node found or not found
    if (data < node->data)
        return find(node->left, data); // Search in the left subtree
    return find(node->right, data);    // Search in the right subtree
}

// Find data in AVL Tree , return nullptr if not found else return pointer to data
template <typename T>
T* AVLTree<T>::find(const T& data) const {
    AVLNode<T>* foundNode = find(root, data);
    return foundNode ? &(foundNode->data) : nullptr;
}

// Performs in-order traversal of the AVL Tree and stores the elements in a vector.
template <typename T>
void AVLTree<T>::inOrderTraversal(AVLNode<T>* node, vector<T>& items) const {
    if (node) {
        inOrderTraversal(node->left, items);  // Traverse left subtree
        items.push_back(node->data);          // Visit node
        inOrderTraversal(node->right, items); // Traverse right subtree
    }
}

// Returns all elements in the AVL Tree in sorted order.
template <typename T>
vector<T> AVLTree<T>::getAllItems() const {
    vector<T> items;
    inOrderTraversal(root, items);
    return items;
}

// Recursively deletes all nodes in the AVL Tree.
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

// Displays all elements in the AVL Tree in sorted order.
template <typename T>
void AVLTree<T>::displayInOrder() const {
    vector<T> items = getAllItems();
    for (const auto& item : items) {
        cout << item << endl;
    }
}