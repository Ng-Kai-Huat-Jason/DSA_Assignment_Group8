#pragma once
#include <iostream>
#include <vector>
using namespace std;

// Node structure for AVL Tree
template <typename T>
struct AVLNode {
    T data;
    AVLNode* left;
    AVLNode* right;
    int height;

    AVLNode(T data) : data(data), left(nullptr), right(nullptr), height(1) {}
};

// AVL Tree class
template <typename T>
class AVLTree {
private:
    AVLNode<T>* root;

    // Helper functions
    AVLNode<T>* insert(AVLNode<T>* node, T data);
    AVLNode<T>* remove(AVLNode<T>* node, const T& data);
    AVLNode<T>* find(AVLNode<T>* node, const T& data) const;
    AVLNode<T>* minValueNode(AVLNode<T>* node) const;
    AVLNode<T>* rotateRight(AVLNode<T>* node);
    AVLNode<T>* rotateLeft(AVLNode<T>* node);
    int getHeight(AVLNode<T>* node) const;
    int getBalanceFactor(AVLNode<T>* node) const;
    void inOrderTraversal(AVLNode<T>* node, vector<T>& items) const;
    void destroy(AVLNode<T>* node);

public:
    AVLTree() : root(nullptr) {}
    ~AVLTree();

    void insert(T data);
    void remove(const T& data);
    T* find(const T& data) const;
    void displayInOrder() const;
    vector<T> getAllItems() const;
};

// Definitions

template <typename T>
int AVLTree<T>::getHeight(AVLNode<T>* node) const {
    return node ? node->height : 0;
}

template <typename T>
int AVLTree<T>::getBalanceFactor(AVLNode<T>* node) const {
    return node ? getHeight(node->left) - getHeight(node->right) : 0;
}

template <typename T>
AVLNode<T>* AVLTree<T>::rotateRight(AVLNode<T>* y) {
    AVLNode<T>* x = y->left;
    AVLNode<T>* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

    return x;
}

template <typename T>
AVLNode<T>* AVLTree<T>::rotateLeft(AVLNode<T>* x) {
    AVLNode<T>* y = x->right;
    AVLNode<T>* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;

    return y;
}

template <typename T>
AVLNode<T>* AVLTree<T>::insert(AVLNode<T>* node, T data) {
    if (!node)
        return new AVLNode<T>(data);

    if (data < node->data)
        node->left = insert(node->left, data);
    else if (data > node->data)
        node->right = insert(node->right, data);
    else
        return node; // Duplicate keys not allowed

    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    int balance = getBalanceFactor(node);

    if (balance > 1 && data < node->left->data)
        return rotateRight(node);
    if (balance < -1 && data > node->right->data)
        return rotateLeft(node);
    if (balance > 1 && data > node->left->data) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (balance < -1 && data < node->right->data) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

template <typename T>
void AVLTree<T>::insert(T data) {
    root = insert(root, data);
}

template <typename T>
AVLNode<T>* AVLTree<T>::minValueNode(AVLNode<T>* node) const {
    AVLNode<T>* current = node;
    while (current && current->left)
        current = current->left;
    return current;
}

template <typename T>
AVLNode<T>* AVLTree<T>::remove(AVLNode<T>* node, const T& data) {
    if (!node)
        return node;

    if (data < node->data)
        node->left = remove(node->left, data);
    else if (data > node->data)
        node->right = remove(node->right, data);
    else {
        if (!node->left || !node->right) {
            AVLNode<T>* temp = node->left ? node->left : node->right;
            if (!temp)
                return nullptr;
            *node = *temp;
            delete temp;
        }
        else {
            AVLNode<T>* temp = minValueNode(node->right);
            node->data = temp->data;
            node->right = remove(node->right, temp->data);
        }
    }

    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    int balance = getBalanceFactor(node);

    if (balance > 1 && getBalanceFactor(node->left) >= 0)
        return rotateRight(node);
    if (balance > 1 && getBalanceFactor(node->left) < 0) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (balance < -1 && getBalanceFactor(node->right) <= 0)
        return rotateLeft(node);
    if (balance < -1 && getBalanceFactor(node->right) > 0) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

template <typename T>
void AVLTree<T>::remove(const T& data) {
    root = remove(root, data);
}

template <typename T>
AVLNode<T>* AVLTree<T>::find(AVLNode<T>* node, const T& data) const {
    if (!node || node->data == data)
        return node;
    if (data < node->data)
        return find(node->left, data);
    return find(node->right, data);
}

template <typename T>
T* AVLTree<T>::find(const T& data) const {
    AVLNode<T>* foundNode = find(root, data);
    return foundNode ? &(foundNode->data) : nullptr;
}

template <typename T>
void AVLTree<T>::inOrderTraversal(AVLNode<T>* node, vector<T>& items) const {
    if (node) {
        inOrderTraversal(node->left, items);
        items.push_back(node->data);
        inOrderTraversal(node->right, items);
    }
}

template <typename T>
vector<T> AVLTree<T>::getAllItems() const {
    vector<T> items;
    inOrderTraversal(root, items);
    return items;
}

template <typename T>
void AVLTree<T>::destroy(AVLNode<T>* node) {
    if (node) {
        destroy(node->left);
        destroy(node->right);
        delete node;
    }
}

template <typename T>
AVLTree<T>::~AVLTree() {
    destroy(root);
}

template <typename T>
void AVLTree<T>::displayInOrder() const {
    vector<T> items = getAllItems();
    for (const auto& item : items) {
        cout << item << endl;
    }
}

