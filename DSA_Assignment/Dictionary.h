#pragma once

#include <vector>
#include <string>
#include <iostream>

using namespace std;

// Using Prime Number for better hash distribution
// 101 is the nearest prime number to 100
const int MAX_SIZE = 101;

class Actor;
class Movie;

template<typename T>
class AVLTree;

// Node structure
template <typename KeyType, typename ValueType>
struct KeyValuePair {
    KeyType key;
    ValueType* value;

    // Node (KeyValuePair) constructor
    KeyValuePair(KeyType key, ValueType* value) : key(key), value(value) {

    }
    bool operator<(const KeyValuePair& other) const {
        return key < other.key;
    }

    bool operator>(const KeyValuePair& other) const {
        return other < *this;
    }

    bool operator==(const KeyValuePair& other) const {
        return key == other.key;
    }

    // And the inequality operator in terms of equality.
    bool operator!=(const KeyValuePair& other) const {
        return !(*this == other);
    }
};

// Overload operator<< for KeyValuePair so that it can be printed.
template <typename KeyType, typename ValueType>
std::ostream& operator<<(std::ostream& os, const KeyValuePair<KeyType, ValueType>& kvp) {
    os << "[" << kvp.key << ", " << kvp.value << "]";
    return os;
}

// Dictionary class
template <typename KeyType, typename ValueType>
class Dictionary {
private:
    AVLTree<KeyValuePair<KeyType, ValueType>>* table[MAX_SIZE];
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

    // Returns a vector of ValueType* from all buckets (existing method)
    vector<ValueType*> getAllItems() const;

    // Returns a vector of Node pointers (each containing both key and value)
    vector<KeyValuePair<KeyType, ValueType>*> getAllNodes() const;

    bool loadFromCSV(const string& fileName, bool isActor);

    // Patches the CSV file (updates records based on current dictionary data).
    bool patchCSV(const string& fileName, bool isActor);
};

