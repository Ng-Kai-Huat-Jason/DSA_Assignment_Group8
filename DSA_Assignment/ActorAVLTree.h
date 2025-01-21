#pragma once
#include<iostream>
#include<string>
#include "Actor.h"
#include "DoublyLinkedList.h"
using namespace std;

struct AVLActorNode {
    Actor* actor;
    AVLActorNode* left;
    AVLActorNode* right;
    int height;
    DoublyLinkedList<AVLActorNode>* linkedList;  // list of related movies
};

// AVL Tree Class for Actors
class ActorAVLTree {
public:
    ActorAVLTree();
    ~ActorAVLTree();

    void insert(Actor* actor);
    void displayInOrder() const;
    void addActorToMovieList(AVLActorNode* actorNode);

private:
    AVLActorNode* root;

    AVLActorNode* insertToAVL(AVLActorNode* node, Actor* actor);
    AVLActorNode* rotateLeft(AVLActorNode* node);
    AVLActorNode* rotateRight(AVLActorNode* node);
    void inOrderTraversal(AVLActorNode* node) const;
    void deleteTree(AVLActorNode* node);
    int getHeight(AVLActorNode* node) const;
    int getBalanceFactor(AVLActorNode* node) const;

    AVLActorNode* getActorNode(string& actorName);

};