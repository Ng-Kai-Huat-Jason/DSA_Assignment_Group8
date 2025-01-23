#include "MovieAVLTree.h"

// constructor
MovieAVLTree::MovieAVLTree() : root(nullptr) {}

// destructor
MovieAVLTree::~MovieAVLTree() {
    deleteTree(root);
}

// insert a movie into the AVL tree
void MovieAVLTree::insert(Movie* movie) {
    root = insert(root, movie);
}

// add an actor to a movie's related actor list
void MovieAVLTree::addActorToMovie(AVLMovieNode* movieNode, AVLActorNode* actorNode) {
    if (movieNode->linkedList == nullptr) {
        movieNode->linkedList = new DoublyLinkedList<AVLMovieNode>();
    }
    movieNode->linkedList->insertAtHead(movieNode);
}

// insert a movie node into the AVL tree
AVLMovieNode* MovieAVLTree::insert(AVLMovieNode* node, Movie* movie) {
    if (node == nullptr) {
        node = new AVLMovieNode();
        node->movie = movie;
        node->left = node->right = nullptr;
        node->height = 1;
    }
    else if (movie->year < node->movie->year) {
        node->left = insert(node->left, movie);
    }
    else if (movie->year > node->movie->year) {
        node->right = insert(node->right, movie);
    }
    else {
        return node;
    }

    // balance the tree
    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    int balance = getBalance(node);

    if (balance > 1 && movie->year < node->left->movie->year) {
        return rightRotate(node);
    }

    if (balance < -1 && movie->year > node->right->movie->year) {
        return leftRotate(node);
    }

    if (balance > 1 && movie->year > node->left->movie->year) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    if (balance < -1 && movie->year < node->right->movie->year) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

// left rotation for AVL balancing
AVLMovieNode* MovieAVLTree::leftRotate(AVLMovieNode* node) {
    AVLMovieNode* newRoot = node->right;
    AVLMovieNode* leftSubtree = newRoot->left;

    newRoot->left = node;
    node->right = leftSubtree;

    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    newRoot->height = 1 + max(getHeight(newRoot->left), getHeight(newRoot->right));

    return newRoot;
}

// right rotation for AVL balancing
AVLMovieNode* MovieAVLTree::rightRotate(AVLMovieNode* node) {
    AVLMovieNode* newRoot = node->left;
    AVLMovieNode* rightSubtree = newRoot->right;

    newRoot->right = node;
    node->left = rightSubtree;

    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    newRoot->height = 1 + max(getHeight(newRoot->left), getHeight(newRoot->right));

    return newRoot;
}

// display movies in order (in ascending year)
void MovieAVLTree::displayInOrder() {
    inorder(root);
}

// recursive inorder traversal of the tree
void MovieAVLTree::inorder(AVLMovieNode* node) {
    if (node == nullptr) return;

    inorder(node->left);
    cout << "Movie: " << node->movie->title << ", Year: " << node->movie->year << endl;
    node->movie->actors.display();  // display related actors for this movie
    inorder(node->right);
}

// get the height of a node
int MovieAVLTree::getHeight(AVLMovieNode* node) {
    if (node == nullptr) return 0;
    return node->height;
}

// get the balance factor of a node
int MovieAVLTree::getBalance(AVLMovieNode* node) {
    if (node == nullptr) return 0;
    return getHeight(node->left) - getHeight(node->right);
}

// delete the entire tree
void MovieAVLTree::deleteTree(AVLMovieNode* node) {
    if (node == nullptr) return;

    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
}

// find movie node by title
AVLMovieNode* MovieAVLTree::findMovieNode(string movieTitle) {
    return findMovieNodeHelper(root, movieTitle);
}

// helper function to search (can implement searching algo)
AVLMovieNode* findMovieNodeHelper(AVLMovieNode* node, string movieTitle) {
    if (node == nullptr) {
        return nullptr;
    }

    // Compare the movie title
    if (node->movie->title == movieTitle) {
        return node;  // Movie found
    }

    // Search left and right subtrees
    if (movieTitle < node->movie->title) {
        return findMovieNodeHelper(node->left, movieTitle);
    }
    else {
        return findMovieNodeHelper(node->right, movieTitle);
    }
}
