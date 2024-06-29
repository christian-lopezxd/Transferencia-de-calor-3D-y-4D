// Función que implementa el método de Gauss-Seidel para resolver un sistema de ecuaciones lineales.
// Parámetros:
//   - Matrix* K: puntero a la matriz de coeficientes del sistema de ecuaciones.
//   - Vector* b: puntero al vector de términos independientes del sistema de ecuaciones.
//   - Vector* T: puntero al vector solución donde se almacenarán los resultados.

void gauss_seidel(Matrix* K, Vector* b, Vector* T) {
  // Obtiene el número de columnas de la matriz K (suponiendo que K es una matriz cuadrada)
  unsigned int n = K->get_ncols();
  float aux_sum = 0.0f; // Variable auxiliar para la suma

  // Inicializa el vector T con valores iniciales, típicamente ceros
  T->init();

  // Itera un máximo de 10000 veces para garantizar la convergencia
  for (int k = 0; k < 10000; k++) {
    // Itera sobre cada fila de la matriz
    for (unsigned int i = 0; i < n; i++) {
      aux_sum = 0.0f; // Reinicia la suma auxiliar para cada fila
      
      // Calcula la suma de K[i][j] * T[j] para todos los j excepto cuando j == i
      for (unsigned int j = 0; j < n; j++) {
        if (j == i) {
          continue; // Omite el elemento diagonal
        }
        aux_sum += K->get(i, j) * T->get(j); // Suma el producto de K[i][j] y T[j]
      }

      // Actualiza el valor de T[i] usando la fórmula de Gauss-Seidel
      T->set((b->get(i) - aux_sum) / K->get(i, i), i);
    }
  }
}
