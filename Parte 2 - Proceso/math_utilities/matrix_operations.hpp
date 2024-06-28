#ifndef MATRIX_OPERATIONS_HPP
#define MATRIX_OPERATIONS_HPP

#include <cmath>
#include "matrix.hpp"
#include "vector.hpp"

void product_scalar_by_matrix(float scalar, Matrix* M, int n, int m, Matrix* R) {
    for (int r = 0; r < n; ++r)
        for (int c = 0; c < m; ++c)
            R->set(scalar * M->get(r, c), r, c);
}

void product_matrix_by_vector(Matrix* M, Vector* V, int n, int m, Vector* R) {
    for (int r = 0; r < n; ++r) {
        float acc = 0;
        for (int c = 0; c < m; ++c)
            acc += M->get(r, c) * V->get(c);
        R->set(acc, r);
    }
}

void product_matrix_by_matrix(Matrix* A, Matrix* B, Matrix* R) {
    int n = A->getNRows(), m = A->getNCols(), p = B->getNRows(), q = B->getNCols();
    if (m == p) {
        R->setSize(n, q);
        R->init();
        for (int r = 0; r < n; r++) {
            for (int c = 0; c < q; c++) {
                float sum = 0;
                for (int k = 0; k < m; k++) {
                    sum += A->get(r, k) * B->get(k, c);
                }
                R->set(sum, r, c);
            }
        }
    } else {
        cerr << "Matrix dimensions mismatch for multiplication." << endl;
        exit(EXIT_FAILURE);
    }
}


float determinant(Matrix* M);

float determinant_auxiliar(Matrix* M) {
    int n = M->getNCols();
    float acc = 0;

    for (int c = 0; c < n; ++c) {
        Matrix clon(n, n);
        M->clone(&clon);
        clon.remove_row(0);
        clon.remove_column(c);
        acc += pow(-1, c) * M->get(0, c) * determinant(&clon);
    }

    return acc;
}

float determinant(Matrix* M) { 
    float ans;
    switch (M->getNCols()) {
        case 1: ans = M->get(0, 0); break;
        case 2: ans = M->get(0, 0) * M->get(1, 1) - M->get(0, 1) * M->get(1, 0); break;
        case 3: ans = M->get(0, 0) * M->get(1, 1) * M->get(2, 2) -
                      M->get(0, 0) * M->get(1, 2) * M->get(2, 1) -
                      M->get(0, 1) * M->get(1, 0) * M->get(2, 2) +
                      M->get(0, 1) * M->get(1, 2) * M->get(2, 0) +
                      M->get(0, 2) * M->get(1, 0) * M->get(2, 1) -
                      M->get(0, 2) * M->get(1, 1) * M->get(2, 0); break;
        default: ans = determinant_auxiliar(M);
    }
    return ans;
}

float get_minor(Matrix* M, int n, int r, int c) {
    Matrix clon(n, n);
    M->clone(&clon);
    clon.remove_row(r);
    clon.remove_column(c);
    return determinant(&clon);
}

void conjugate_matrix(Matrix* M, int n, Matrix* C) {
    for (int r = 0; r < n; ++r)
        for (int c = 0; c < n; ++c)
            C->set(pow(-1, r + c) * get_minor(M, n, r, c), r, c);
}

void transpose(Matrix* M, int n, int m, Matrix* T) {
    for (int r = 0; r < n; r++) {
        for (int c = 0; c < m; c++) {
            T->set(M->get(r, c), c, r);
        }
    }
}


void calculate_inverse(Matrix* M, int n, Matrix* R) {
    cout << "\t\tCalculating determinant...\n\n";
    float detM = determinant(M);
    
    cout << "\t\tCalculating Conjugate Matrix...\n\n";
    Matrix Conj(n, n);
    conjugate_matrix(M, n, &Conj);
    
    cout << "\t\tCalculating Adjunct Matrix...\n\n";
    Matrix Adj(n, n);
    transpose(&Conj, n, n, &Adj);
    
    cout << "\t\tMultiplying the Adjunct by the determinant...\n\n";
    product_scalar_by_matrix(1 / detM, &Adj, n, n, R);
}

void calculate_inverse_Cholesky(Matrix* A, int n, Matrix* X) {
    Matrix L(n, n), Y(n, n);
    float acc;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j) {
                acc = 0;
                for (int k = 0; k < j; ++k) {
                    acc += pow(L.get(j, k), 2);
                }
                float rad = A->get(j, j) - acc;
                L.set((rad <= 0 ? 0.000001 : sqrt(rad)), j, j);
            } else if (i > j) {
                acc = 0;
                for (int k = 0; k < j; ++k) {
                    acc += L.get(i, k) * L.get(j, k);
                }
                L.set((1 / L.get(j, j)) * (A->get(i, j) - acc), i, j);
            } else {
                L.set(0, i, j);
            }
        }
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j) {
                Y.set(1 / L.get(i, i), i, i);
            } else if (i > j) {
                acc = 0;
                for (int k = j; k < i; ++k) {
                    acc += L.get(i, k) * Y.get(k, j);
                }
                Y.set(-(1 / L.get(i, i)) * acc, i, j);
            } else {
                Y.set(0, i, j);
            }
        }
    }

    for (int i = n - 1; i >= 0; --i) {
        for (int j = 0; j < n; ++j) {
            acc = 0;
            for (int k = i + 1; k < n; ++k) {
                acc += L.get(k, i) * X->get(k, j);
            }
            X->set((1 / L.get(i, i)) * (Y.get(i, j) - acc), i, j);
        }
    }
}

#endif
