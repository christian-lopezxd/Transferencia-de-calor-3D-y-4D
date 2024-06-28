#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <iostream>
#include <cstdlib>

using namespace std;

class Matrix {
private:
    int nrows, ncols;
    float** data;

    void create() {
        data = (float**)malloc(sizeof(float*) * nrows);
        for (int r = 0; r < nrows; ++r) {
            data[r] = (float*)malloc(sizeof(float) * ncols);
            if (data[r] == nullptr) {
                cerr << "Memory allocation failed!" << endl;
                exit(EXIT_FAILURE);
            }
        }
    }

public:
    Matrix() : nrows(0), ncols(0), data(nullptr) {}

    Matrix(int rows, int cols) : nrows(rows), ncols(cols) {
        create();
        init();
    }

    ~Matrix() {
        for (int r = 0; r < nrows; ++r) {
            free(data[r]);
        }
        free(data);
    }

    void init() {
        for (int r = 0; r < nrows; ++r)
            for (int c = 0; c < ncols; ++c)
                data[r][c] = 0;
    }

    void setSize(int rows, int cols) {
        if (data != nullptr) {
            for (int r = 0; r < nrows; ++r) {
                free(data[r]);
            }
            free(data);
        }
        nrows = rows;
        ncols = cols;
        create();
        init();
    }

    int getNRows() const {
        return nrows;
    }

    int getNCols() const {
        return ncols;
    }

    void set(float value, int row, int col) {
        data[row][col] = value;
    }

    void add(float value, int row, int col) {
        data[row][col] += value;
    }

    float get(int row, int col) const {
        return data[row][col];
    }

    void remove_row(int row) {
        int neo_index = 0;
        float** neo_data = (float**)malloc(sizeof(float*) * (nrows - 1));
        for (int i = 0; i < nrows; ++i) {
            if (i != row) {
                neo_data[neo_index] = data[i];
                ++neo_index;
            }
        }
        free(data[row]);
        free(data);
        data = neo_data;
        --nrows;
    }

    void remove_column(int col) {
        int neo_index = 0;
        float** neo_data = (float**)malloc(sizeof(float*) * nrows);
        for (int r = 0; r < nrows; ++r) {
            neo_data[r] = (float*)malloc(sizeof(float) * (ncols - 1));
        }

        for (int r = 0; r < nrows; ++r) {
            for (int c = 0; c < ncols; ++c) {
                if (c != col) {
                    neo_data[r][neo_index] = data[r][c];
                    ++neo_index;
                }
            }
            neo_index = 0;
        }
        for (int r = 0; r < nrows; ++r) {
            free(data[r]);
        }
        free(data);
        data = neo_data;
        --ncols;
    }

    void clone(Matrix* other) const {
        other->setSize(nrows, ncols);
        for (int r = 0; r < nrows; ++r) {
            for (int c = 0; c < ncols; ++c) {
                other->set(data[r][c], r, c);
            }
        }
    }

    void show() const {
        cout << "[ ";
        for (int r = 0; r < nrows; ++r) {
            cout << "[ " << data[r][0];
            for (int c = 1; c < ncols; ++c) {
                cout << ", " << data[r][c];
            }
            cout << " ] ";
        }
        cout << " ]" << endl;
    }
};

#endif
