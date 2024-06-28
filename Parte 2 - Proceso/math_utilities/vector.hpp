#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <iostream>
#include <cstdlib>

using namespace std;

class Vector {
private:
    int size;
    float* data;

    void create() {
        data = (float*)malloc(sizeof(float) * size);
        if (data == nullptr) {
            cerr << "Memory allocation failed!" << endl;
            exit(EXIT_FAILURE);
        }
    }

public:
    Vector() : size(0), data(nullptr) {}

    Vector(int num_values) : size(num_values) {
        create();
        init();
    }

    ~Vector() {
        if (data != nullptr) {
            free(data);
        }
    }

    void init() {
        for (int i = 0; i < size; ++i) {
            data[i] = 0.0f;
        }
    }

    void setSize(int num_values) {
        if (data != nullptr) {
            free(data);
        }
        size = num_values;
        create();
        init();
    }

    int getSize() const {
        return size;
    }

    void set(float value, int position) {
        if (position >= 0 && position < size) {
            data[position] = value;
        } else {
            cerr << "Index out of bounds!" << endl;
        }
    }

    void add(float value, int position) {
        if (position >= 0 && position < size) {
            data[position] += value;
        } else {
            cerr << "Index out of bounds!" << endl;
        }
    }

    float get(int position) const {
        if (position >= 0 && position < size) {
            return data[position];
        } else {
            cerr << "Index out of bounds!" << endl;
            return 0.0f; // Valor por defecto en caso de error
        }
    }

    void removeRow(int row) {
        if (row < 0 || row >= size) {
            cerr << "Index out of bounds!" << endl;
            return;
        }

        int newSize = size - 1;
        float* newData = (float*)malloc(sizeof(float) * newSize);
        if (newData == nullptr) {
            cerr << "Memory allocation failed!" << endl;
            exit(EXIT_FAILURE);
        }

        for (int i = 0, j = 0; i < size; ++i) {
            if (i != row) {
                newData[j++] = data[i];
            }
        }

        free(data);
        data = newData;
        size = newSize;
    }

    void show() const {
        cout << "[ ";
        for (int i = 0; i < size; ++i) {
            cout << data[i];
            if (i < size - 1) {
                cout << "; ";
            }
        }
        cout << " ]" << endl;
    }
};

#endif
