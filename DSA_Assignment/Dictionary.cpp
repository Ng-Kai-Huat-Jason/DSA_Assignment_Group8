
#include "Dictionary.h"
#include "Actor.h"  
#include "Movie.h" 
#include <cctype> 
#include <sstream>

using namespace std;

// Node constructor
template <typename KeyType, typename ValueType>
Node<KeyType, ValueType>::Node(KeyType key, ValueType* value)
    : key(key), value(value), next(nullptr) {
    if (!value) {
        cerr << "Error: Node created with null value for key: " << key << endl;
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

// Custom hash function for string keys
template <typename KeyType, typename ValueType>
unsigned long Dictionary<KeyType, ValueType>::hash(const KeyType& key) const {
    unsigned long hashValue = 0;
    for (char c : key) {
        hashValue = (hashValue * 31 + c) % MAX_SIZE;
    }
    return hashValue;
}

// Add an item to the dictionary
template <typename KeyType, typename ValueType>
bool Dictionary<KeyType, ValueType>::add(const KeyType& key, ValueType* value) {

    // Check for null value
    if (!value) {
        cerr << "Error: Trying to add a null value for key: " << key << endl;
        return false;
    }

    // Check for duplicates
    unsigned long index = hash(key);
    Node<KeyType, ValueType>* current = table[index];

    // Check for duplicates
    while (current) {
        if (current->key == key) {
            cerr << "Error: Duplicate key detected: " << key << endl;
            return false;
        }
        current = current->next;
    }

    // Create a new node and insert it at the beginning of the linked list
    Node<KeyType, ValueType>* newNode = new Node<KeyType, ValueType>(key, value);
    newNode->next = table[index];
    table[index] = newNode;
    size++;
    return true;
}

// Remove an item from the dictionary
template <typename KeyType, typename ValueType>
bool Dictionary<KeyType, ValueType>::remove(const KeyType& key) {
    
	// Find the item and remove it
    unsigned long index = hash(key);
    Node<KeyType, ValueType>* current = table[index];
    Node<KeyType, ValueType>* prev = nullptr;

	// Traverse the linked list
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

// Get an item from the dictionary
template <typename KeyType, typename ValueType>
ValueType* Dictionary<KeyType, ValueType>::get(const KeyType& key) const {

	// Find the item and return it
    unsigned long index = hash(key);
    Node<KeyType, ValueType>* current = table[index];

	// Traverse the linked list
    while (current) {
        if (current->key == key) {
            return current->value;
        }
        current = current->next;
    }

	// Return nullptr if not found
    return nullptr; 
}

// Check if the dictionary is empty
template <typename KeyType, typename ValueType>
bool Dictionary<KeyType, ValueType>::isEmpty() const {
    return size == 0;
}

// Get the size of the dictionary
template <typename KeyType, typename ValueType>
int Dictionary<KeyType, ValueType>::getSize() const {
    return size;
}

// Print the dictionary
template <typename KeyType, typename ValueType>
void Dictionary<KeyType, ValueType>::print() const {
	// Traverse the hash table and print all items
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
	// Traverse the hash table and store all items in a vector
    vector<ValueType*> items;
    for (int i = 0; i < MAX_SIZE; ++i) {
        Node<KeyType, ValueType>* current = table[i];
        while (current) {
            items.push_back(current->value);
            current = current->next;
        }
    }

	// Return the vector
    return items;
}

// Returns a vector of Node pointers (each node contains both key and value)
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
