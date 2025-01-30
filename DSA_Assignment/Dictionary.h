// Dictionary.h
#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <vector>
#include <string>
#include <iostream>

using namespace std;

const int MAX_SIZE = 101; // Prime number for better hash distribution

// Forward declarations
class Actor;
class Movie;

// Node structure
template <typename KeyType, typename ValueType>
struct Node {
    KeyType key;
    ValueType* value;
    Node* next;

    Node(KeyType key, ValueType* value);
};

// Dictionary class
template <typename KeyType, typename ValueType>
class Dictionary {
private:
    Node<KeyType, ValueType>* table[MAX_SIZE];
    int size;


    unsigned long hash(const KeyType& key) const;

public:
    Dictionary();
    ~Dictionary();

    bool add(const KeyType& key, ValueType* value);
    bool remove(const KeyType& key);
    ValueType* get(const KeyType& key) const;
    bool isEmpty() const;
    int getSize() const;
    void print() const;

    vector<ValueType*> getAllItems() const;
};

#endif // DICTIONARY_H
