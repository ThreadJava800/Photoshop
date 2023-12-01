#ifndef _LISTpp_h_
#define _LISTpp_h_

#include "iostream"

namespace ThreadJava800_List {

template<typename T>
class List {
private:
    T*     values   = nullptr;
    size_t capacity = 0;
    size_t size     = 0;

public:
    List() :
        values  (nullptr),
        capacity(1),
        size    (0)
        {
            values = (T*) calloc(sizeof(T), 1);
        }

    List(size_t _capacity) :
        values  (nullptr),
        capacity(_capacity),
        size    (0)
        {
            values = (T*) calloc(sizeof(T), _capacity);
        }

    ~List() {
        if (values) free(values);

        capacity = 0;
        size     = 0;
        values   = nullptr;
    }

    size_t getCapacity() {
        return capacity;
    }

    size_t getSize() {
        return size;
    }

    T* getCArray() {
        return values;
    }

    void clear() {
        size = 0;
        // values = (T*) realloc(values, sizeof(T));
    }

    bool exists(T elem) {

        for (size_t i = 0; i < size; i++) {
            if (values[i] == elem) return true;
        }

        return false;
    }

    void pushBack(T elem) {

        if (capacity <= size) {
            values = (T*) realloc(values, capacity * 2 * sizeof(T));

            capacity = 2 * capacity;
        }

        values[size] = elem;
        size++;
    }

    void pushFront(T elem) {
        if (capacity <= size) {
            values = (T*) realloc(values, capacity * 2 * sizeof(T));

            capacity = 2 * capacity;
        }

        for (long i = size - 1; i >= 0; i--) {
            values[i + 1] = values[i];
        }
        values[0] = elem;
        size++;
    }

    void swapWithEnd(T elem) {
        long elemInd = -1;
        T swapEl;
        for (size_t i = 0; i < size; i++) {
            if (values[i] == elem) {
                elemInd = i;
                swapEl  = elem;
                break;
            }
        }

        if (elemInd == -1) return;

        for (size_t i = elemInd; i < size - 1; i++) {
            values[i] = values[i + 1];
        }
        values[size - 1] = swapEl;
    }

    void pushBack(List<T>* elem) {
        if (!elem) return;

        size_t elemSize = elem->getSize();
        for (size_t i = 0; i < elemSize; i++) {
            pushBack((*elem)[i]);
        }

        delete elem;
    }

    T pop() {
        return values[--size];
    }

    T popFront() {
        T popElem = values[0];
        swapWithEnd(popElem);
        pop();

        return popElem;
    }

    void remove(long index) {

        for (long i = index; i < long(size) - 1; i++) {
            values[i] = values[i + 1];
        }

        // if (size < capacity / 2 - 1) {
        //     values = (T*) realloc(values, capacity / 2 * sizeof(T));

        //     capacity /= 2;
        // }
        size--;
    }

    void insert(T value, size_t index) {
        pushBack(value);
        for (long i = long(size) - 1; i >= index + 1; i--) {
            T tmp         = values[i];
            values[i]     = values[i - 1];
            values[i - 1] = tmp;
        }
    }

    T& operator[](const size_t index) {
        return values[index];
    }

    void updateElem(T elem, size_t index) {
        values[index] = elem;
    }

    void print() {
        for (size_t i = 0; i < size; i++) {
            std::cout << values[i] << ' ';
        }
        std::cout << '\n';
    }

    List<T>* createCopy() {
        List<T>* copyList = new List<T>();
        for (size_t i = 0; i < size; i++) copyList->pushBack(values[i]);

        return copyList;
    }
};

};

#endif