#include "list.hpp"

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
