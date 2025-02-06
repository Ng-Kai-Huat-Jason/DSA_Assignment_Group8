#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <vector>
#include <string>
#include <iostream>

using namespace std;

// Using Prime Number for better hash distribution
// 101 is the nearest prime number to 100
const int MAX_SIZE = 101;

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
    virtual ~Dictionary();  // Make the destructor virtual for polymorphism

    // Basic operations
    bool add(const KeyType& key, ValueType* value);
    bool remove(const KeyType& key);
    ValueType* get(const KeyType& key) const;
    bool isEmpty() const;
    int getSize() const;
    void print() const;

    // Returns a vector of ValueType* (existing method)
    vector<ValueType*> getAllItems() const;

    // Returns a vector of Node pointers (each containing both key and value)
    vector<Node<KeyType, ValueType>*> getAllNodes() const;

    bool loadFromCSV(const string& fileName, bool isActor);

    // Patches the CSV file (updates records based on current dictionary data).
    bool patchCSV(const string& fileName, bool isActor);

};

#endif // DICTIONARY_H
