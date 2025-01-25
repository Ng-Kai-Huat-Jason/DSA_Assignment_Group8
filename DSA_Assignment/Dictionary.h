#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <string>
#include <iostream>
using namespace std;

const int MAX_SIZE = 101; // Prime number for better hash distribution

template <typename KeyType, typename ValueType>
struct Node {
    KeyType key;
    ValueType* value;
    Node* next;

    Node(KeyType key, ValueType* value) : key(key), value(value), next(nullptr) {}
};

template <typename KeyType, typename ValueType>
class Dictionary {
private:
    Node<KeyType, ValueType>* table[MAX_SIZE];
    int size;

    int hash(KeyType key) const;

public:
    Dictionary();
    ~Dictionary();

    bool add(KeyType key, ValueType* value);
    bool remove(KeyType key);
    ValueType* get(KeyType key);
    bool isEmpty() const;
    int getSize() const;

    void print() const;
};

#endif // DICTIONARY_H
