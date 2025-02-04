#include "Dictionary.h"
#include "Actor.h"  // Ensure Actor is fully defined
#include "Movie.h"  // Ensure Movie is fully defined
#include <cctype> // For character handling (if needed)
#include <sstream>

// Node constructor
template <typename KeyType, typename ValueType>
Node<KeyType, ValueType>::Node(KeyType key, ValueType* value)
    : key(key), value(value), next(nullptr) {
    if (!value) {
        std::cerr << "Error: Node created with null value for key: " << key << std::endl;
    }
}

// Constructor for Dictionary
template <typename KeyType, typename ValueType>
Dictionary<KeyType, ValueType>::Dictionary() : size(0) {
    for (int i = 0; i < MAX_SIZE; ++i) {
        table[i] = nullptr;
    }
}

// Destructor for Dictionary
template <typename KeyType, typename ValueType>
Dictionary<KeyType, ValueType>::~Dictionary() {
    for (int i = 0; i < MAX_SIZE; ++i) {
        Node<KeyType, ValueType>* current = table[i];
        while (current) {
            Node<KeyType, ValueType>* toDelete = current;
            current = current->next;
            if (toDelete->value) {
                delete toDelete->value;
            }
            delete toDelete;
        }
    }
}

// Custom hash function for std::string keys
template <typename KeyType, typename ValueType>
unsigned long Dictionary<KeyType, ValueType>::hash(const KeyType& key) const {
    unsigned long hashValue = 0;
    for (char c : key) {
        hashValue = (hashValue * 31 + c) % MAX_SIZE;
    }
    return hashValue;
}

// Add an item
template <typename KeyType, typename ValueType>
bool Dictionary<KeyType, ValueType>::add(const KeyType& key, ValueType* value) {
    if (!value) {
        std::cerr << "Error: Trying to add a null value for key: " << key << std::endl;
        return false;
    }

    unsigned long index = hash(key);
    Node<KeyType, ValueType>* current = table[index];

    // Check for duplicates
    while (current) {
        if (current->key == key) {
            std::cerr << "Error: Duplicate key detected: " << key << std::endl;
            return false;
        }
        current = current->next;
    }

    // Add new node at the beginning
    Node<KeyType, ValueType>* newNode = new Node<KeyType, ValueType>(key, value);
    newNode->next = table[index];
    table[index] = newNode;
    size++;
    return true;
}

// Remove an item
template <typename KeyType, typename ValueType>
bool Dictionary<KeyType, ValueType>::remove(const KeyType& key) {
    unsigned long index = hash(key);
    Node<KeyType, ValueType>* current = table[index];
    Node<KeyType, ValueType>* prev = nullptr;

    while (current) {
        if (current->key == key) {
            if (prev) {
                prev->next = current->next;
            }
            else {
                table[index] = current->next;
            }
            delete current->value;
            delete current;
            size--;
            return true;
        }
        prev = current;
        current = current->next;
    }
    return false;
}

// Get an item
template <typename KeyType, typename ValueType>
ValueType* Dictionary<KeyType, ValueType>::get(const KeyType& key) const {
    unsigned long index = hash(key);
    Node<KeyType, ValueType>* current = table[index];
    while (current) {
        if (current->key == key) {
            return current->value;
        }
        current = current->next;
    }
    return nullptr; // Not found
}

// Check if empty
template <typename KeyType, typename ValueType>
bool Dictionary<KeyType, ValueType>::isEmpty() const {
    return size == 0;
}

// Get size
template <typename KeyType, typename ValueType>
int Dictionary<KeyType, ValueType>::getSize() const {
    return size;
}

// Print the dictionary (for debugging)
template <typename KeyType, typename ValueType>
void Dictionary<KeyType, ValueType>::print() const {
    for (int i = 0; i < MAX_SIZE; ++i) {
        Node<KeyType, ValueType>* current = table[i];
        while (current) {
            cout << "Key: " << current->key << ", Value Address: " << current->value << endl;
            current = current->next;
        }
    }
}

// getAllItems: Returns a vector of values
template <typename KeyType, typename ValueType>
vector<ValueType*> Dictionary<KeyType, ValueType>::getAllItems() const {
    vector<ValueType*> items;
    for (int i = 0; i < MAX_SIZE; ++i) {
        Node<KeyType, ValueType>* current = table[i];
        while (current) {
            items.push_back(current->value);
            current = current->next;
        }
    }
    return items;
}

// getAllNodes: Returns a vector of Node pointers (each node contains both key and value)
template <typename KeyType, typename ValueType>
vector<Node<KeyType, ValueType>*> Dictionary<KeyType, ValueType>::getAllNodes() const {
    vector<Node<KeyType, ValueType>*> nodes;
    for (int i = 0; i < MAX_SIZE; ++i) {
        Node<KeyType, ValueType>* current = table[i];
        while (current) {
            nodes.push_back(current);
            current = current->next;
        }
    }
    return nodes;
}

// Explicit template instantiation
template class Dictionary<string, Actor>;
template class Dictionary<string, Movie>;
