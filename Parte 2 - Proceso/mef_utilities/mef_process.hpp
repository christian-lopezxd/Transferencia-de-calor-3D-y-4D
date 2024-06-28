#ifndef MEF_PROCESS_HPP
#define MEF_PROCESS_HPP

#include "../geometry/mesh.hpp"
#include "../math_utilities/matrix_operations.hpp"
#include "../math_utilities/vector.hpp"


float calculate_local_area(float x1, float y1, float x2, float y2, float x3, float y3){
    float A = abs((x1*y2 + x2*y3 + x3*y1) - (x1*y3 + x2*y1 + x3*y2))/2;
    return ((A==0)?0.000001:A);
}

float calculate_local_jacobian(float x1, float y1, float x2, float y2, float x3, float y3){
    float J = (x2 - x1)*(y3 - y1) - (x3 - x1)*(y2 - y1);
    return ((J==0)?0.000001:J);
}

void calculate_B(Matrix* B) {
    B->set(-1, 0, 0);
    B->set(1, 0, 1);
    B->set(0, 0, 2);
    B->set(-1, 1, 0);
    B->set(0, 1, 1);
    B->set(1, 1, 2);
}


void calculate_local_A(Matrix* A, float x1, float y1, float x2, float y2, float x3, float y3) {
    A->set(y3 - y1, 0, 0);
    A->set(x1 - x3, 0, 1);
    A->set(y1 - y2, 1, 0);
    A->set(x2 - x1, 1, 1);
}


// Crear matriz local K


void create_local_K(Matrix* K, int element_id, Mesh* M) {
    cout << "\t\tEntering create_local_K for Element " << element_id + 1 << endl;

    Element* element = M->getElement(element_id);
    if (element == nullptr) {
        cerr << "\t\tElement " << element_id + 1 << " is null!" << endl;
        return;
    }

    Node* node1 = element->getNode1();
    Node* node2 = element->getNode2();
    Node* node3 = element->getNode3();

    if (node1 == nullptr || node2 == nullptr || node3 == nullptr) {
        cerr << "\t\tOne or more nodes are null for element " << element_id + 1 << endl;
        return;
    }

    float x1 = node1->x_coordinate;
    float y1 = node1->y_coordinate;
    float x2 = node2->x_coordinate;
    float y2 = node2->y_coordinate;
    float x3 = node3->x_coordinate;
    float y3 = node3->y_coordinate;

    cout << "\t\tCoordinates: (" << x1 << ", " << y1 << "), (" << x2 << ", " << y2 << "), (" << x3 << ", " << y3 << ")\n";

    float area = calculate_local_area(x1, y1, x2, y2, x3, y3);
    if (area <= 0) {
        cerr << "\t\tInvalid area calculated: " << area << endl;
        return;
    }

    float jacobian = calculate_local_jacobian(x1, y1, x2, y2, x3, y3);
    if (jacobian == 0) {
        cerr << "\t\tInvalid jacobian calculated!" << endl;
        return;
    }

    cout << "\t\tArea: " << area << ", Jacobian: " << jacobian << endl;

    // Crear matrices B y A
    Matrix B(2, 3), A(2, 2);
    calculate_B(&B);
    calculate_local_A(&A, x1, y1, x2, y2, x3, y3);

    Matrix Bt(3, 2), At(2, 2);
    transpose(&B, 2, 3, &Bt);
    transpose(&A, 2, 2, &At);

    Matrix res1, res2, res3;
    product_matrix_by_matrix(&A, &B, &res1);
    product_matrix_by_matrix(&At, &res1, &res2);
    product_matrix_by_matrix(&Bt, &res2, &res3);
    product_scalar_by_matrix(M->getProblemData(THERMAL_CONDUCTIVITY) * area / (jacobian * jacobian), &res3, 3, 3, K);

    cout << "\t\tFinished creating local K for element " << element_id + 1 << endl;
}



// Crear vector local b
void create_local_b(Vector* b, int element_id, Mesh* M) {
    b->setSize(3);

    float Q = M->getProblemData(HEAT_SOURCE);
    float x1 = M->getElement(element_id)->getNode1()->x_coordinate, y1 = M->getElement(element_id)->getNode1()->y_coordinate,
          x2 = M->getElement(element_id)->getNode2()->x_coordinate, y2 = M->getElement(element_id)->getNode2()->y_coordinate,
          x3 = M->getElement(element_id)->getNode3()->x_coordinate, y3 = M->getElement(element_id)->getNode3()->y_coordinate;

    float J = calculate_local_jacobian(x1, y1, x2, y2, x3, y3);

    b->set(Q * J / 6, 0);
    b->set(Q * J / 6, 1);
    b->set(Q * J / 6, 2);
}

// Crear sistemas locales
void create_local_systems(Matrix* Ks, Vector* bs, int num_elements, Mesh* M) {
    for (int e = 0; e < num_elements; e++) {
        cout << "\tCreating local system for Element " << e + 1 << "...\n\n";
        create_local_K(&Ks[e], e, M);
        create_local_b(&bs[e], e, M);
    }
}

void assembly_K(Matrix* K, Matrix* local_K, int index1, int index2, int index3){
    K->add(local_K->get(0,0),index1,index1);    K->add(local_K->get(0,1),index1,index2);    K->add(local_K->get(0,2),index1,index3);
    K->add(local_K->get(1,0),index2,index1);    K->add(local_K->get(1,1),index2,index2);    K->add(local_K->get(1,2),index2,index3);
    K->add(local_K->get(2,0),index3,index1);    K->add(local_K->get(2,1),index3,index2);    K->add(local_K->get(2,2),index3,index3);
}

void assembly_b(Vector* b, Vector* local_b, int index1, int index2, int index3){
    b->add(local_b->get(0),index1);
    b->add(local_b->get(1),index2);
    b->add(local_b->get(2),index3);
}
// Ensamblar matriz global K
void assembly(Matrix* K, Vector* b, Matrix* Ks, Vector* bs, int num_elements, Mesh* M) {
    K->init();
    b->init();

    for (int e = 0; e < num_elements; e++) {
        cout << "\tAssembling for Element " << e + 1 << "...\n\n";
        int index1 = M->getElement(e)->getNode1()->id - 1;
        int index2 = M->getElement(e)->getNode2()->id - 1;
        int index3 = M->getElement(e)->getNode3()->id - 1;

        assembly_K(K, &Ks[e], index1, index2, index3);
        assembly_b(b, &bs[e], index1, index2, index3);
    }
}

// Aplicar condiciones de frontera de Neumann
void apply_neumann_boundary_conditions(Vector* b, Mesh* M) {
    int num_conditions = M->getQuantity(NUM_NEUMANN);

    for (int c = 0; c < num_conditions; c++) {
        Condition* cond = M->getNeumannCondition(c);
        int index = cond->getNode()->id - 1;
        b->add(cond->getValue(), index);
    }
}

void add_column_to_RHS(Matrix* K, Vector* b, int col, float T_bar){
    for(int r = 0; r < K->getNRows(); r++)
        b->add(-T_bar*K->get(r,col),r);
}


void solve_system(Matrix* K, Vector* b, Vector* T, int mode) {
    int n = K->getNRows();
    Matrix Kinv(n, n);

    if (mode == 1) {
        cout << "\tCalculating inverse of global matrix K using standard method...\n\n";
        calculate_inverse(K, n, &Kinv);
    } else if (mode == 2) {
        cout << "\tCalculating inverse of global matrix K using Cholesky method...\n\n";
        calculate_inverse_Cholesky(K, n, &Kinv);
    } else {
        cerr << "Invalid mode for solving system. Use 1 for standard inverse, 2 for Cholesky.\n";
        exit(EXIT_FAILURE);
    }

    product_matrix_by_vector(&Kinv, b, n, n, T);
}

// Aplicar condiciones de frontera de Dirichlet
void apply_dirichlet_boundary_conditions(Matrix* K, Vector* b, Mesh* M) {
    int num_conditions = M->getQuantity(NUM_DIRICHLET);
    int previous_removed = 0;

    for (int c = 0; c < num_conditions; c++) {
        Condition* cond = M->getDirichletCondition(c);
        int index = cond->getNode()->id - 1 - previous_removed;
        float cond_value = cond->getValue();

        K->remove_row(index);
        b->removeRow(index);
        add_column_to_RHS(K, b, index, cond_value);
        K->remove_column(index);

        previous_removed++;
    }
}


// Fusionar resultados con condiciones de Dirichlet
void merge_results_with_dirichlet(Vector* T, Vector* Tf, int n, Mesh* M) {
    int num_dirichlet = M->getQuantity(NUM_DIRICHLET);

    int cont_dirichlet = 0;
    int cont_T = 0;
    for (int i = 0; i < n; i++) {
        if (M->doesNodeHaveDirichletCondition(i + 1)) {
            Condition* cond = M->getDirichletCondition(cont_dirichlet);
            cont_dirichlet++;
            float cond_value = cond->getValue();
            Tf->set(cond_value, i);
        } else {
            Tf->set(T->get(cont_T), i);
            cont_T++;
        }
    }
}

#endif
