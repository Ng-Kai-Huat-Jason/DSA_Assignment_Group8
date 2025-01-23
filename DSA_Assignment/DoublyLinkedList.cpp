#include "DoublyLinkedList.h"
#include <iostream>

template <typename T>
DoublyLinkedList<T>::DoublyLinkedList() : head(nullptr), tail(nullptr) {}

template <typename T>
DoublyLinkedList<T>::~DoublyLinkedList() {
    Node* current = head;
    while (current) {
        Node* next = current->next;
        delete current;
        current = next; 
    }
}

template <typename T>
void DoublyLinkedList<T>::insertAtHead(T* data) {
    Node* newNode = new Node(data);
    if (head == nullptr) {
        head = tail = newNode;
    }
    else {
        newNode->next = head;
        head->prev = newNode;
        head = newNode;
    }
}

template <typename T>
void DoublyLinkedList<T>::insertAtTail(T* data) {
    Node* newNode = new Node(data);
    if (tail == nullptr) {
        head = tail = newNode;
    }
    else {
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }
}

template <typename T>
void DoublyLinkedList<T>::display() const {
    Node* current = head;
    while (current) {
        std::cout << "Data: " << current->data << std::endl;
        current = current->next;
    }
}
