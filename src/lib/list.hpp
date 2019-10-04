#ifndef LIST_HPP
#define LIST_HPP

#include <kernel/logger/logger.hpp>

template <typename T>
class ListNode {
public:
    ListNode(const T &e) : data(e), prev(nullptr), next(nullptr) {}
    
    T data;
    ListNode<T>* prev;
    ListNode<T>* next;
};

template <typename T>
class ListIterator {
public:
    ListIterator() : curr(nullptr) {}
    ListIterator(ListNode<T> &e) : curr(e) {}

    ListIterator<T>& operator++();
    ListIterator<T> operator++(int);
    T& operator*();
    T* operator->();
    bool operator!=(const ListIterator<T> &rhs);

private:
    ListNode<T>* curr;
};

template <typename T>
class List {
public:
    List() : head(nullptr), tail(nullptr), size_(0) {}

    ListNode<T>* insert(ListIterator<T> pos,  const T& value);
    ListNode<T>* append(const T& value) {
        return insert(this->end(), value);
    }

    ListIterator<T> begin() { return ListIterator<T>(*tail); }
    ListIterator<T> end() { return ListIterator<T>(); }
    int size() { return size_; }
    void print() {
        for (auto it = this->begin(); it != this->end(); it++) {
            logger::msg_info(*it);
        }
    }

private:
    ListNode<T>* head;
    ListNode<T>* tail;
    int size_;    
};

template <typename T>
ListIterator<T>& ListIterator<T>::operator++() {
    if (curr != nullptr) {
        curr = curr->next;
    }
}

template <typename T>
ListIterator<T> ListIterator<T>::operator++(int n) {
    if (curr != nullptr) {
        curr = curr->next;
    }
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

template <typename T>
ListNode<T>* List<T>::insert(ListIterator<T> pos,  const T& value) {
    ListNode<T> *n = new ListNode<T>(value);
    size_++;

    n->next = pos->curr;
    n->data = value;
    
    if (pos) {
        n->prev = pos->prev;
        pos->prev = n;
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