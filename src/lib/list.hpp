template <typename T>
class ListNode {
    ListNode(const T &e) : data(e), prev(nullptr), next(nullptr) {}
    
    T data;
    ListNode<T>* prev;
    ListNode<T>* next;
};

template <typename T>
class ListIterator {
    ListIterator() : curr(nullptr) {}
    ListIterator(const ListNode<T> &e) : curr(e) {}
    ListNode<T>* curr;

    ListIterator<T>& operator++();
    ListIterator<T> operator++(int);
    T& operator*();
    T* operator->();
    bool operator!=(const ListIterator<T> &rhs);

    ListIterator<T> begin();
    ListIterator<T> end();
};

template <typename T>
class List {
    List() : head(nullptr), tail(nullptr), size(0) {}
    ListNode<T>* insert(ListIterator<T> pos,  const T& value);

private:
    ListNode<T>* head;
    ListNode<T>* tail;
    int size;    
};

