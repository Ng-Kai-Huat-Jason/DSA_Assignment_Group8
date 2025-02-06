// Binary Search Tree 
// Each Node contains a pointer to a actor or movie objects

#pragma once
#include <iostream>
using namespace std;

template <typename T>

// AVL Node structure
struct AVLNode {
    T* data;
    AVLNode* left;
    AVLNode* right;
    int height;

    AVLNode(T* data) : data(data), left(nullptr), right(nullptr), height(1) {}
};

template <typename T>
class AVLTree {
private:
    AVLNode<T>* root;

	// Helper functions
    AVLNode<T>* insert(AVLNode<T>* node, T* data);
    AVLNode<T>* rotateRight(AVLNode<T>* node);
    AVLNode<T>* rotateLeft(AVLNode<T>* node);
    
    // Get Height of AVL Tree
    int getHeight(AVLNode<T>* node) const;

	// Get Balance Factor of AVL Tree
    int getBalanceFactor(AVLNode<T>* node) const;

	// In-order traversal
    void inOrderTraversal(AVLNode<T>* node) const;

	// Destroy AVL Tree
    void destroy(AVLNode<T>* node); 

public:
    AVLTree() : root(nullptr) {}
    ~AVLTree(); 

    void insert(T* data);
    void displayInOrder() const;
};
