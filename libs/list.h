#ifndef _LISTpp_h_
#define _LISTpp_h_

#include "iostream"

#ifndef ON_ERROR
#define ON_ERROR(expr, errStr, retVal) {                 \
    if (expr) {                                           \
        fprintf(stderr, "FATAL ERROR: %s\n", errStr);      \
        return retVal;                                      \
    }                                                        \
}                                                             \

#endif

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
            ON_ERROR(!values, "Unable to allocate memory",);
        }

    List(size_t _capacity) :
        values  (nullptr),
        capacity(_capacity),
        size    (0)
        {
            values = (T*) calloc(sizeof(T), _capacity);
            ON_ERROR(!values, "Unable to allocate memory",);
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

    void pushBack(T elem) {
        ON_ERROR(!values, "List was null",);

        if (capacity <= size) {
            values = (T*) realloc(values, capacity * 2 * sizeof(T));
            ON_ERROR(!values, "Unable to realloc mem",);

            capacity = 2 * capacity;
        }

        values[size] = elem;
        size++;
    }

    void remove(size_t index) {
        ON_ERROR(!values, "List was null",);

        for (size_t i = index; i < size - 1; i++) {
            values[i] = values[i + 1];
        }

        if (size < capacity / 2 - 1) {
            values = (T*) realloc(values, capacity / 2 * sizeof(T));
            ON_ERROR(!values, "Unable to realloc mem",);

            capacity /= 2;
        }
        size--;
    }

    T& operator[](const size_t index) {
        return values[index];
    }

    void updateElem(T elem, size_t index) {
        ON_ERROR(!values, "List was null",);

        values[index] = elem;
    }

    void print() {
        for (size_t i = 0; i < size; i++) {
            std::cout << values[i] << ' ';
        }
        std::cout << '\n';
    }
};

#endif