#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "Movie.h"
#include "MovieAVLTree.h"

using namespace std;

class MovieHashTable {
private:
    vector<MovieAVLTree*> table;  // array of AVL trees
    int size;  // size of the hash table

    int hashFunction(int year) const;  // hash function to map year to a bucket

public:
    MovieHashTable(int size);
    ~MovieHashTable();

    // insert movie into the hash table
    void insertMovie(Movie* movie);  

    // add actor to movie
    void addActorToMovie(Movie* movie, Actor* actor); 

    // display movies by year
    void displayMoviesByYear(int year) const;  

    // display movies sorted by title 
    void displayMoviesInYearSorted(int year) const; 
};
