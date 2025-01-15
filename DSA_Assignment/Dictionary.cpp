#include "Dictionary.h"

// Hash function
template <typename KeyType, typename ItemType>
int Dictionary<KeyType, ItemType>::hash(KeyType key) const {
    int hashValue = 0;
    for (char ch : key) {
        hashValue = (hashValue * 31 + ch) % MAX_SIZE;
    }
    return hashValue;
}

// Constructor
template <typename KeyType, typename ItemType>
Dictionary<KeyType, ItemType>::Dictionary() : size(0) {
    for (int i = 0; i < MAX_SIZE; i++) {
        items[i] = nullptr;
    }
}

// Destructor
template <typename KeyType, typename ItemType>
Dictionary<KeyType, ItemType>::~Dictionary() {
    for (int i = 0; i < MAX_SIZE; i++) {
        Node<KeyType, ItemType>* current = items[i];
        while (current) {
            Node<KeyType, ItemType>* temp = current;
            current = current->next;
            delete temp;
        }
    }
}

// Add a new item
template <typename KeyType, typename ItemType>
bool Dictionary<KeyType, ItemType>::add(KeyType newKey, ItemType newItem) {
    int index = hash(newKey);

    Node<KeyType, ItemType>* newNode = new Node<KeyType, ItemType>{ newKey, newItem, items[index] };
    items[index] = newNode;
    size++;
    return true;
}

// Remove an item
template <typename KeyType, typename ItemType>
bool Dictionary<KeyType, ItemType>::remove(KeyType key) {
    int index = hash(key);
    Node<KeyType, ItemType>* current = items[index];
    Node<KeyType, ItemType>* prev = nullptr;

    while (current) {
        if (current->key == key) {
            if (prev) {
                prev->next = current->next;
            }
            else {
                items[index] = current->next;
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
template <typename KeyType, typename ItemType>
ItemType* Dictionary<KeyType, ItemType>::get(KeyType key) {
    int index = hash(key);
    Node<KeyType, ItemType>* current = items[index];

    while (current) {
        if (current->key == key) {
            return &current->item;
        }
        current = current->next;
    }
    return nullptr;
}

// Check if the dictionary is empty
template <typename KeyType, typename ItemType>
bool Dictionary<KeyType, ItemType>::isEmpty() const {
    return size == 0;
}

// Get the number of items
template <typename KeyType, typename ItemType>
int Dictionary<KeyType, ItemType>::getLength() const {
    return size;
}

// Print all items
template <typename KeyType, typename ItemType>
void Dictionary<KeyType, ItemType>::print() const {
    for (int i = 0; i < MAX_SIZE; i++) {
        Node<KeyType, ItemType>* current = items[i];
        while (current) {
            cout << "Key: " << current->key << ", Item: " << endl;
            current = current->next;
        }
    }
}

// Explicit template instantiations for Actor, Movie, and Cast
template class Dictionary<string, Actor>;
template class Dictionary<string, Movie>;
template class Dictionary<string, Cast>;
