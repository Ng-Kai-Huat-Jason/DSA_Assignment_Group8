#include "Dictionary.h"
#include "Actor.h"
#include "Movie.h"

// Constructor
template <typename KeyType, typename ValueType>
Dictionary<KeyType, ValueType>::Dictionary() : size(0) {
    for (int i = 0; i < MAX_SIZE; i++) {
        table[i] = nullptr;
    }
}

// Destructor
template <typename KeyType, typename ValueType>
Dictionary<KeyType, ValueType>::~Dictionary() {
    for (int i = 0; i < MAX_SIZE; i++) {
        Node<KeyType, ValueType>* current = table[i];
        while (current) {
            Node<KeyType, ValueType>* toDelete = current;
            current = current->next;
            delete toDelete->value;
            delete toDelete;
        }
    }
}

// Hash function
template <typename KeyType, typename ValueType>
int Dictionary<KeyType, ValueType>::hash(KeyType key) const {
    int hashValue = 0;
    for (char ch : key) {
        hashValue = (hashValue * 31 + ch) % MAX_SIZE;
    }
    return hashValue;
}

// Add an item
template <typename KeyType, typename ValueType>
bool Dictionary<KeyType, ValueType>::add(KeyType key, ValueType* value) {
    int index = hash(key);
    
    Node<KeyType, ValueType>* current = table[index];
    while (current) {
        if (current->key == key) {
            cout << "Error: Duplicate key detected!" << endl;
            return false;
        }
        current = current->next;
    }

    Node<KeyType, ValueType>* newNode = new Node<KeyType, ValueType>(key, value);
    newNode->next = table[index];
    table[index] = newNode;
    size++;
    return true;
}

// Remove an item
template <typename KeyType, typename ValueType>
bool Dictionary<KeyType, ValueType>::remove(KeyType key) {
    int index = hash(key);
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
ValueType* Dictionary<KeyType, ValueType>::get(KeyType key) {
    int index = hash(key);
    Node<KeyType, ValueType>* current = table[index];
    while (current) {
        if (current->key == key) {
            return current->value;
        }
        current = current->next;
    }
    return nullptr;
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

// Print the dictionary
template <typename KeyType, typename ValueType>
void Dictionary<KeyType, ValueType>::print() const {
    for (int i = 0; i < MAX_SIZE; i++) {
        Node<KeyType, ValueType>* current = table[i];
        while (current) {
            cout << "Key: " << current->key << ", Value: (Address: " << *(current->value) << ")" << endl;
            current = current->next;
        }
    }
}

// Retrieve all items as a vector
template <typename KeyType, typename ValueType>
std::vector<ValueType*> Dictionary<KeyType, ValueType>::getAllItems() const {
    std::vector<ValueType*> items;
    for (int i = 0; i < MAX_SIZE; ++i) {
        Node<KeyType, ValueType>* current = table[i];
        while (current) {
            items.push_back(current->value);
            current = current->next;
        }
    }
    return items;
}

// Explicit template instantiation
template class Dictionary<string, Actor>;
template class Dictionary<string, Movie>;
