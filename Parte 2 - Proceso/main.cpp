#include <iostream>
#include <cstdlib>
#include <string>
#include "geometry/mesh.hpp"
#include "math_utilities/matrix_operations.hpp"
#include "mef_utilities/mef_process.hpp"
#include "gid/input_output.hpp"

using namespace std;

int main(int argc, char** argv) {
    if (argc != 3) {
        cout << "Incorrect use of the program, it must be: mef filename method\n";
        exit(EXIT_FAILURE);
    }

    Mesh M;

    cout << "Reading geometry and mesh data...\n\n";
    string filename(argv[1]);
    readInput(filename, &M);

    short num_nodes = M.getQuantity(NUM_NODES);
    short num_elements = M.getQuantity(NUM_ELEMENTS);
    Matrix K(num_nodes, num_nodes);
    Matrix* local_Ks = new Matrix[num_elements];
    Vector b(num_nodes);
    Vector* local_bs = new Vector[num_elements];

if (local_Ks == nullptr || local_bs == nullptr) {
    cerr << "Memory allocation failed for local systems." << endl;
    exit(EXIT_FAILURE);
}


    cout << "Creating local systems...\n\n";
    create_local_systems(local_Ks, local_bs, num_elements, &M);

    cout << "Performing Assembly...\n\n";
    assembly(&K, &b, local_Ks, local_bs, num_elements, &M);

    cout << "Applying Neumann Boundary Conditions...\n\n";
    apply_neumann_boundary_conditions(&b, &M);

    cout << "Applying Dirichlet Boundary Conditions...\n\n";
    apply_dirichlet_boundary_conditions(&K, &b, &M);

    cout << "Solving global system...\n\n";
    Vector T(b.getSize());
    Vector T_full(num_nodes);
    solve_system(&K, &b, &T, atoi(argv[2]));

    cout << "Preparing results...\n\n";
    merge_results_with_dirichlet(&T, &T_full, num_nodes, &M);

    cout << "Writing output file...\n\n";
    writeOutput(filename, &T_full);

    // Liberar la memoria asignada dinámicamente
    delete[] local_Ks;
    delete[] local_bs;

    return 0;
}
