#include <cmath>

//Decidimos implementar el metodo de Gauss Seidel para que la compilación no tarde un montón

void gauss_seidel(Matrix* K, Vector* b, Vector* T) {
  unsigned int n = K->get_ncols();
  float aux_sum = 0.0f;
  T->init();
  for (int k = 0; k < 10000; k++) {
    for (unsigned int i = 0; i < n; i++) {
      aux_sum = 0.0f;
      for (unsigned int j = 0; j < n; j++) {
        if (j == i) {
          continue;
        }
        aux_sum += K->get(i, j) * T->get(j);
      }
      T->set((b->get(i) - aux_sum) / K->get(i, i), i);
    }
  }
}