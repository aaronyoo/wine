#ifndef LIST_HPP
#define LIST_HPP

#include <kernel/logger/logger.hpp>

//===================
// ListNode
//===================
template <typename T>
class ListNode {
public:
    ListNode(const T &e) : data(e), prev(nullptr), next(nullptr) {}
    
    T data;
    ListNode<T>* prev;
    ListNode<T>* next;
};

//===================
// ListIterator
//===================
template <typename T>
class ListIterator {
public:
    ListIterator() : curr(nullptr) {}
    ListIterator(ListNode<T> &e) : curr(&e) {}

    ListIterator<T>& operator++();
    ListIterator<T> operator++(int);
    T& operator*();
    T* operator->();
    bool operator!=(const ListIterator<T> &rhs);
    ListNode<T>* get_curr() { return curr; }

private:
    ListNode<T>* curr;
};

template <typename T>
ListIterator<T>& ListIterator<T>::operator++() {
    if (curr != nullptr) {
        curr = curr->next;
    }
}

template <typename T>
ListIterator<T> ListIterator<T>::operator++(int n) {
    curr = curr->next;
}

template <typename T>
T& ListIterator<T>::operator*() {
    return curr->data;
}

template <typename T>
T* ListIterator<T>::operator->() {
    return &(curr->data);
}

template <typename T>
bool ListIterator<T>::operator!=(const ListIterator<T> &rhs) {
    return this->curr != rhs.curr;
}

//===================
// List
//===================
template <typename T>
class List {
public:
    // Constructors
    List() : head(nullptr), tail(nullptr), size_(0) {}

    // Externally defined functions
    ListNode<T>* insert(ListIterator<T> pos,  const T& value);
    ListNode<T>* append(const T& value);

    // Internally defined functions
    ListIterator<T> begin() { return ListIterator<T>(*head); }
    ListIterator<T> end() { return ListIterator<T>(); }
    int size() { return size_; }

private:
    ListNode<T>* head;
    ListNode<T>* tail;
    int size_;    
};

template <typename T>
ListNode<T>* List<T>::append(const T& value) {
    insert(this->end(), value);
}

template <typename T>
ListNode<T>* List<T>::insert(ListIterator<T> pos,  const T& value) {
    ListNode<T> *n = new ListNode<T>(value);
    size_++;

    n->next = pos.get_curr();
    n->data = value;
    
    if (pos != this->end()) {
        n->prev = pos.get_curr()->prev;
        pos.get_curr()->prev = n;
    } else {
        // pos is at the end of the list
        n->prev = tail;
        tail = n;
    }

    if (n->prev) {
        n->prev->next = n;
    } else {
        // pos is at the beginning of the list
        head = n;
    }

    return n;
}

#endif // LIST_HPP