#pragma once
#include <iostream>
using namespace std;

template <typename T>
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
    int getHeight(AVLNode<T>* node) const;
    int getBalanceFactor(AVLNode<T>* node) const;
    void inOrderTraversal(AVLNode<T>* node) const;
    void destroy(AVLNode<T>* node); // Helper to destroy tree nodes

public:
    AVLTree() : root(nullptr) {}
    ~AVLTree(); // Destructor to clean up memory

    void insert(T* data);
    void displayInOrder() const;
};
