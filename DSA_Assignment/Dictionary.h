#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <string>
#include <vector>
#include <iostream>

using namespace std;

// Constants
const int MAX_SIZE = 101;

// Types
struct Actor {
    string id;
    string name;
    int birth;
};

struct Movie {
    string id;
    string title;
    string plot;
    int year;
};

struct Cast {
    string person_id;
    string movie_id;
};

template <typename KeyType, typename ItemType>
struct Node {
    KeyType key;
    ItemType item;
    Node* next;
};

// Dictionary Class
template <typename KeyType, typename ItemType>
class Dictionary {
private:
    Node<KeyType, ItemType>* items[MAX_SIZE];
    int size;

    int hash(KeyType key) const;

public:
    Dictionary();
    ~Dictionary();

    bool add(KeyType newKey, ItemType newItem);
    bool remove(KeyType key);
    ItemType* get(KeyType key);
    bool isEmpty() const;
    int getLength() const;

    // Utility methods for display
    void print() const;
};

#endif // DICTIONARY_H
