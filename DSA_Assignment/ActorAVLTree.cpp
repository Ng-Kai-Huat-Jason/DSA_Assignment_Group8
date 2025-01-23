#include "ActorAVLTree.h"
#include <iostream>

ActorAVLTree::ActorAVLTree() : root(nullptr) {}

ActorAVLTree::~ActorAVLTree() {
    deleteTree(root);
}

void ActorAVLTree::insert(Actor* actor) {
    root = insertToAVL(root, actor);
}

void ActorAVLTree::addActorToMovieList(AVLActorNode* actorNode) {
    // Logic to add actor to movie's list
    // This would typically involve iterating through movies and adding them to the actor's linked list
    if (actorNode->linkedList == nullptr) {
        actorNode->linkedList = new DoublyLinkedList<AVLActorNode>();
    }
    actorNode->linkedList->insertAtHead(actorNode);  // Example logic
}

void ActorAVLTree::displayInOrder() const {
    inOrderTraversal(root);
}

AVLActorNode* ActorAVLTree::insertToAVL(AVLActorNode* node, Actor* actor) {
    if (!node) {
        node = new AVLActorNode{ actor, nullptr, nullptr, 1, nullptr };
        return node;
    }
    if (actor->getName() < node->actor->getName() || actor->getName() == node->actor->getName()) {
        node->left = insertToAVL(node->left, actor);
    }
    else {
        node->right = insertToAVL(node->right, actor);
    }

    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    int balance = getBalanceFactor(node);

    if (balance > 1 && (actor->getName() < node->left->actor->getName() || actor->getName() == node->left->actor->getName()))
        return rotateRight(node);
    if (balance < -1 && actor->getName() > node->right->actor->getName())
        return rotateLeft(node);
    if (balance > 1 && actor->getName() > node->left->actor->getName()) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (balance < -1 && actor->getName() < node->right->actor->getName()) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

AVLActorNode* ActorAVLTree::rotateLeft(AVLActorNode* node) {
    AVLActorNode* newRoot = node->right;
    node->right = newRoot->left;
    newRoot->left = node;

    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    newRoot->height = 1 + max(getHeight(newRoot->left), getHeight(newRoot->right));

    return newRoot;
}

AVLActorNode* ActorAVLTree::rotateRight(AVLActorNode* node) {
    AVLActorNode* newRoot = node->left;
    node->left = newRoot->right;
    newRoot->right = node;

    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    newRoot->height = 1 + max(getHeight(newRoot->left), getHeight(newRoot->right));

    return newRoot;
}

void ActorAVLTree::inOrderTraversal(AVLActorNode* node) const {
    if (node) {
        inOrderTraversal(node->left);
        std::cout << "Actor: " << node->actor->getName() << ", Birth Year: " << node->actor->getBirthYear() << std::endl;
        inOrderTraversal(node->right);
    }
}

void ActorAVLTree::deleteTree(AVLActorNode* node) {
    if (node) {
        deleteTree(node->left);
        deleteTree(node->right);
        delete node->actor;
        delete node;
    }
}

int ActorAVLTree::getHeight(AVLActorNode* node) const {
    return node ? node->height : 0;
}

int ActorAVLTree::getBalanceFactor(AVLActorNode* node) const {
    return node ? getHeight(node->left) - getHeight(node->right) : 0;
}

AVLActorNode* ActorAVLTree::getActorNode(string& actorName) {
    return getActorNodeHelper(root, actorName);
}

AVLActorNode* getActorNodeHelper(AVLActorNode* node, string& actorName) {
    // base case: node is null or actor is found
    if (node == nullptr || node->actor->getName() == actorName) {
        return node;
    }

    if (actorName < node->actor->getName()) {
        return getActorNodeHelper(node->left, actorName);
    }

    // If actor name is lexicographically larger, search in the right subtree
    return getActorNodeHelper(node->right, actorName);
}