#include "MovieHashTable.h"

MovieHashTable::MovieHashTable(int size) : size(size) {
    table.resize(size, nullptr);
}

MovieHashTable::~MovieHashTable() {
    for (int i = 0; i < size; ++i) {
        if (table[i]) {
            delete table[i];
        }
    }
}

int MovieHashTable::hashFunction(int year) const {
    return year % size; 
}

void MovieHashTable::insertMovie(Movie* movie) {
    int bucketIndex = hashFunction(movie->year); // get the bucket index using the year
    MovieAVLTree* tree = table[bucketIndex];

    if (tree == nullptr) {
        // If the tree is empty, create a new AVL tree and insert the movie
        tree = new MovieAVLTree();
        table[bucketIndex] = tree;
    }
    tree->insert(movie);  // Insert the movie into the AVL tree
}

// add an actor to the movie's related actors list
void MovieHashTable::addActorToMovie(Movie* movie, Actor* actor) {
    int bucketIndex = hashFunction(movie->year);
    MovieAVLTree* tree = table[bucketIndex];

    if (tree != nullptr) {
        // Find the movie node in the AVL tree
        AVLMovieNode* movieNode = tree->findMovieNode();
        if (movieNode != nullptr) {
            // find the actor node in the ActorAVLTree

            // redo logic 
                // if actor node is not nullptr
                    // add the actor to the movie's related actors list
        }
    }
}

void MovieHashTable::displayMoviesByYear(int year) const {
    int index = hashFunction(year);
    if (table[index]) {
        table[index]->displayInOrder();
    }
    else {
        cout << "No movies found for the year " << year << endl;
    }
}

void MovieHashTable::displayMoviesInYearSorted(int year) const {
    int index = hashFunction(year);
    if (table[index]) {
        table[index]->displayInOrder();
    }
    else {
        cout << "No movies found for the year " << year << endl;
    }
}
