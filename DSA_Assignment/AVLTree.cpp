#include "AVLTree.h"
#include "Actor.h"
#include "Movie.h"

// Helper function to get the height of a node
template <typename T>
int AVLTree<T>::getHeight(AVLNode<T>* node) const {
    return node ? node->height : 0;
}

// Helper function to calculate the balance factor of a node
template <typename T>
int AVLTree<T>::getBalanceFactor(AVLNode<T>* node) const {
    if (!node) return 0;
    return getHeight(node->left) - getHeight(node->right);
}

// Helper function to rotate right
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

    return x;
}

// Helper function to rotate left
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

    return y;
}

// Helper function to insert into the AVL tree
template <typename T>
AVLNode<T>* AVLTree<T>::insert(AVLNode<T>* node, T* data) {
    if (!node) return new AVLNode<T>(data);

    if (*data < *(node->data)) {
        node->left = insert(node->left, data);
    }
    else if (*data > *(node->data)) {
        node->right = insert(node->right, data);
    }
    else {
        return node; // Duplicate keys not allowed
    }

    // Update height
    node->height = max(getHeight(node->left), getHeight(node->right)) + 1;

    // Get balance factor
    int balance = getBalanceFactor(node);

    // Perform rotations to maintain balance
    if (balance > 1 && *data < *(node->left->data)) return rotateRight(node);
    if (balance < -1 && *data > *(node->right->data)) return rotateLeft(node);
    if (balance > 1 && *data > *(node->left->data)) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (balance < -1 && *data < *(node->right->data)) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

// Public insert method
template <typename T>
void AVLTree<T>::insert(T* data) {
    root = insert(root, data);
}

// Helper function for in-order traversal
template <typename T>
void AVLTree<T>::inOrderTraversal(AVLNode<T>* node) const {
    if (!node) return;

    inOrderTraversal(node->left);
    cout << *(node->data) << endl; // Assumes T has an overloaded << operator
    inOrderTraversal(node->right);
}

// Public in-order traversal method
template <typename T>
void AVLTree<T>::displayInOrder() const {
    inOrderTraversal(root);
}

// Helper function to destroy tree nodes
template <typename T>
void AVLTree<T>::destroy(AVLNode<T>* node) {
    if (!node) return;

    destroy(node->left);
    destroy(node->right);
    delete node;
}

// Destructor to clean up the tree
template <typename T>
AVLTree<T>::~AVLTree() {
    destroy(root);
}

// Explicit template instantiation
template class AVLTree<Actor>;
template class AVLTree<Movie>;
