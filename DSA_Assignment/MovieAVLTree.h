#pragma once
#include<iostream>
#include<string>
#include "Movie.h"
#include "DoublyLinkedList.h"
#include "ActorAVLTree.h"

using namespace std;

struct AVLMovieNode {
    Movie* movie;
    AVLMovieNode* left;
    AVLMovieNode* right;
    int height;
    DoublyLinkedList<AVLMovieNode>* linkedList;  // list of pointers to related actors
};

class MovieAVLTree
{
private:
    AVLMovieNode* root;

public:
    MovieAVLTree();
    ~MovieAVLTree();

    // insert movie to tree
    void insert(Movie* movie);
    void displayInOrder();
    void deleteTree(AVLMovieNode* node);
    void addActorToMovie(AVLMovieNode* movieNode, AVLActorNode* actorNode);

    AVLMovieNode* insert(AVLMovieNode* node, Movie* movie);
    AVLMovieNode* leftRotate(AVLMovieNode* node);
    AVLMovieNode* rightRotate(AVLMovieNode* node);
    void inorder(AVLMovieNode* node);
    int getHeight(AVLMovieNode* node);
    int getBalance(AVLMovieNode* node);
    AVLMovieNode* findMovieNode(string movieTitle);
};
