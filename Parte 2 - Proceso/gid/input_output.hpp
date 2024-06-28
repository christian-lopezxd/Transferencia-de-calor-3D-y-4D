#ifndef INPUT_OUTPUT_HPP
#define INPUT_OUTPUT_HPP

#include <fstream>
#include "../geometry/mesh.hpp"

using namespace std;

void readInput(const string& filename, Mesh* mesh) {
    ifstream dat_file(filename + ".dat");
    if (!dat_file.is_open()) {
        cerr << "Error opening file: " << filename << ".dat" << endl;
        exit(EXIT_FAILURE);
    }

    float k, Q, T_bar, T_hat;
    int num_nodes, num_elements, num_dirichlet, num_neumann;

    dat_file >> k >> Q >> T_bar >> T_hat >> num_nodes >> num_elements >> num_dirichlet >> num_neumann;
    mesh->setProblemData(k, Q);
    mesh->setQuantities(num_nodes, num_elements, num_dirichlet, num_neumann);
    mesh->initArrays();

    for (int i = 0; i < num_nodes; ++i) {
        int id;
        float x, y;
        dat_file >> id >> x >> y;
        cout << "Reading node " << id << " with coordinates (" << x << ", " << y << ")" << endl;
        mesh->insertNode(id, x, y);
    }

    for (int i = 0; i < num_elements; ++i) {
        int id, node1_id, node2_id, node3_id;
        dat_file >> id >> node1_id >> node2_id >> node3_id;
        cout << "Reading element " << id << " with nodes " << node1_id << ", " << node2_id << ", " << node3_id << endl;
        mesh->insertElement(new Element(id, mesh->getNode(node1_id), mesh->getNode(node2_id), mesh->getNode(node3_id)), i);
    }

    for (int i = 0; i < num_dirichlet; ++i) {
        int id;
        dat_file >> id;
        mesh->insertDirichletCondition(new Condition(mesh->getNode(id), T_bar), i);
    }

    for (int i = 0; i < num_neumann; ++i) {
        int id;
        dat_file >> id;
        mesh->insertNeumannCondition(new Condition(mesh->getNode(id), T_hat), i);
    }

    dat_file.close();
}


void writeOutput(const string& filename, Vector* T) {
    ofstream res_file(filename + ".post.res");
    
    res_file << "GiD Post Results File 1.0\n";
    res_file << "Result \"Temperature\" \"Load Case 1\" 1 Scalar OnNodes\n";
    res_file << "ComponentNames \"T\"\n";
    res_file << "Values\n";

    int n = T->getSize();
    for (int i = 0; i < n; ++i) {
        res_file << i + 1 << "     " << T->get(i) << "\n";
    }

    res_file << "End values\n";
    res_file.close();
}

#endif
