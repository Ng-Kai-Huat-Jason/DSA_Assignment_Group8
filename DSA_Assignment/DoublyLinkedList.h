#pragma once
#include <string>
using namespace std;

template <typename T>
class DoublyLinkedList {

public:
    struct Node {
        T* data;
        Node* prev;
        Node* next;
        Node(T* data) : data(data), prev(nullptr), next(nullptr) {}
    };
    DoublyLinkedList();
    ~DoublyLinkedList();

    void insertAtHead(T* data);
    void insertAtTail(T* data);
    void display() const;
private:
    Node* head;
    Node* tail;
};
