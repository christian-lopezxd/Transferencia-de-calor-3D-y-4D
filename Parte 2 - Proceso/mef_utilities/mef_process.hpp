// Esta función calcula el volumen local de un elemento tetraédrico.
// Calcula la matriz V que contiene las diferencias de coordenadas entre los nodos.
// Calcula el determinante de V y lo divide entre 6 para obtener el volumen.

float calculate_local_volume(
    float x1, float y1, float z1,
    float x2, float y2, float z2,
    float x3, float y3, float z3,
    float x4, float y4, float z4
){
    Matrix V(3,3);
    V.set(x2-x1, 0,0); V.set(y2-y1, 0,1); V.set(z2-z1, 0,2);
    V.set(x3-x1, 1,0); V.set(y3-y1, 1,1); V.set(z3-z1, 1,2);
    V.set(x4-x1, 2,0); V.set(y4-y1, 2,1); V.set(z4-z1, 2,2);
    return abs(determinant(&V))/6;
}

// Esta función calcula el jacobiano local del elemento tetraédrico.

float calculate_local_jacobian(
    float x1, float y1, float z1,
    float x2, float y2, float z2,
    float x3, float y3, float z3,
    float x4, float y4, float z4
){
    return (x2-x1)*(y3-y1)*(z4-z1)-(x2-x1)*(y4-y1)*(z3-z1)-(x3-x1)*(y2-y1)*(z4-z1)+(x3-x1)*(y4-y1)*(z2-z1)+(x4-x1)*(y2-y1)*(z3-z1)-(x4-x1)*(y3-y1)*(z2-z1);
}


// Esta función calcula la matriz B 

void calculate_B(Matrix* B){
    B->set(-1, 0, 0); B->set(1, 0, 1); B->set(0, 0, 2); B->set(0, 0, 3);
    B->set(-1, 1, 0); B->set(0, 1, 1); B->set(1, 1, 2); B->set(0, 1, 3);
    B->set(-1, 2, 0); B->set(0, 2, 1); B->set(0, 2, 2); B->set(1, 2, 3); 
}

// Calcula la matriz A
// Recibe las coordenadas de los 4 nodos del elemento.

void calculate_local_A(Matrix* A, 
    float x1, float y1, float z1,
    float x2, float y2, float z2,
    float x3, float y3, float z3,
    float x4, float y4, float z4
){
    A->set((y3-y1)*(z4-z1)-(y4-y1)*(z3-z1), 0, 0); A->set(-(x3-x1)*(z4-z1)+(x4-x1)*(z3-z1), 0, 1); A->set((x3-x1)*(y4-y1)-(x4-x1)*(y3-y1), 0, 2);
    A->set(-(y2-y1)*(z4-z1)+(y4-y1)*(z2-z1), 1, 0); A->set((x2-x1)*(z4-z1)-(x4-x1)*(z2-z1), 1, 1); A->set(-(x2-x1)*(y4-y1)+(x4-x1)*(y2-y1), 1, 2);
    A->set((y2-y1)*(z3-z1)-(y3-y1)*(z2-z1), 2, 0); A->set(-(x2-x1)*(z3-z1)+(x3-x1)*(z2-z1), 2, 1); A->set((x2-x1)*(y3-y1)-(x3-x1)*(y2-y1), 2, 2);
}

void create_local_K(Matrix* K, long element_id, Mesh* M){
    K->set_size(4,4);
// Obtiene la conductividad térmica del problema desde el objeto Mesh

    float k = M->get_problem_data(THERMAL_CONDUCTIVITY);
    // Obtiene las coordenadas de los nodos del elemento

    float x1 = M->get_element(element_id)->get_node1()->get_x_coordinate(), y1 = M->get_element(element_id)->get_node1()->get_y_coordinate(), z1 = M->get_element(element_id)->get_node1()->get_z_coordinate(),
          x2 = M->get_element(element_id)->get_node2()->get_x_coordinate(), y2 = M->get_element(element_id)->get_node2()->get_y_coordinate(), z2 = M->get_element(element_id)->get_node2()->get_z_coordinate(),
          x3 = M->get_element(element_id)->get_node3()->get_x_coordinate(), y3 = M->get_element(element_id)->get_node3()->get_y_coordinate(), z3 = M->get_element(element_id)->get_node3()->get_z_coordinate(),
          x4 = M->get_element(element_id)->get_node4()->get_x_coordinate(), y4 = M->get_element(element_id)->get_node4()->get_y_coordinate(), z4 = M->get_element(element_id)->get_node4()->get_z_coordinate();
          // Calcula el volumen y el jacobiano local del elemento  
    float Volume = calculate_local_volume(
        x1,y1,z1,
        x2,y2,z2,
        x3,y3,z3,
        x4,y4,z4
    );
    float J = calculate_local_jacobian(
        x1, y1, z1, 
        x2, y2, z2,
        x3, y3, z3,
        x4, y4, z4
    );

    Matrix B(3,4), A(3,3);
    calculate_B(&B);
    calculate_local_A(&A, 
        x1, y1, z1,
        x2, y2, z2,
        x3, y3, z3,
        x4, y4, z4
    );
    //B.show(); A.show();

    Matrix Bt(4,3), At(3,3);
    transpose(&B,3,4,&Bt);
    transpose(&A,3,3,&At);
    //Bt.show(); At.show();

// Declaracion de las matrices auxiliares para el cálculo
    Matrix res1, res2, res3;
    product_matrix_by_matrix(&A,&B,&res1);
    product_matrix_by_matrix(&At,&res1,&res2);
    product_matrix_by_matrix(&Bt,&res2,&res3);
    product_scalar_by_matrix(k*Volume/(J*J),&res3,4,4,K);

    //cout << "\t\tLocal matrix created for Element " << element_id+1 << ": "; K->show(); cout << "\n";
}

// Esta función crea el vector local b
void create_local_b(Vector* b, long element_id, Mesh* M){
    b->set_size(4);

    float Q = M->get_problem_data(HEAT_SOURCE);
    float x1 = M->get_element(element_id)->get_node1()->get_x_coordinate(), y1 = M->get_element(element_id)->get_node1()->get_y_coordinate(), z1 = M->get_element(element_id)->get_node1()->get_z_coordinate(),
          x2 = M->get_element(element_id)->get_node2()->get_x_coordinate(), y2 = M->get_element(element_id)->get_node2()->get_y_coordinate(), z2 = M->get_element(element_id)->get_node2()->get_z_coordinate(),
          x3 = M->get_element(element_id)->get_node3()->get_x_coordinate(), y3 = M->get_element(element_id)->get_node3()->get_y_coordinate(), z3 = M->get_element(element_id)->get_node3()->get_z_coordinate(),
          x4 = M->get_element(element_id)->get_node4()->get_x_coordinate(), y4 = M->get_element(element_id)->get_node4()->get_y_coordinate(), z4 = M->get_element(element_id)->get_node4()->get_z_coordinate();
    float J = calculate_local_jacobian(
        x1, y1, z1, 
        x2, y2, z2,
        x3, y3, z3,
        x4, y4, z4
    );

    b->set(Q*J/24,0);
    b->set(Q*J/24,1);
    b->set(Q*J/24,2);
    b->set(Q*J/24,3);

    //cout << "\t\tLocal vector created for Element " << element_id+1 << ": "; b->show(); cout << "\n";
}

// Esta función crea los sistemas locales
void create_local_systems(Matrix* Ks, Vector* bs, long num_elements, Mesh* M){
    for(int e = 0; e < num_elements; e++){
        cout << "\tCreating local system for Element " << e+1 << "...\n\n";
        create_local_K(&Ks[e],e,M);
        create_local_b(&bs[e],e,M);
    }
}


// Se ensambla la matriz global K
void assembly_K(Matrix* K, Matrix* local_K, long index1, long index2, int index3, int index4){
    K->add(local_K->get(0,0),index1,index1);    K->add(local_K->get(0,1),index1,index2);    K->add(local_K->get(0,2),index1,index3);    K->add(local_K->get(0,3),index1,index4);
    K->add(local_K->get(1,0),index2,index1);    K->add(local_K->get(1,1),index2,index2);    K->add(local_K->get(1,2),index2,index3);    K->add(local_K->get(1,3),index2,index4);
    K->add(local_K->get(2,0),index3,index1);    K->add(local_K->get(2,1),index3,index2);    K->add(local_K->get(2,2),index3,index3);    K->add(local_K->get(2,3),index3,index4);
    K->add(local_K->get(3,0),index4,index1);    K->add(local_K->get(3,1),index4,index2);    K->add(local_K->get(3,2),index4,index3);    K->add(local_K->get(3,3),index4,index4);
}

// Lo mismo para el vector global b
void assembly_b(Vector* b, Vector* local_b, long index1, long index2, int index3, int index4){
    b->add(local_b->get(0),index1);
    b->add(local_b->get(1),index2);
    b->add(local_b->get(2),index3);
    b->add(local_b->get(3),index4);
}

void assembly(Matrix* K, Vector* b, Matrix* Ks, Vector* bs, long num_elements, Mesh* M){
    K->init();
    b->init();
    //K->show(); b->show();

    for(int e = 0; e < num_elements; e++){
        cout << "\tAssembling for Element " << e+1 << "...\n\n";
        long index1 = M->get_element(e)->get_node1()->get_ID() - 1;
        long index2 = M->get_element(e)->get_node2()->get_ID() - 1;
        long index3 = M->get_element(e)->get_node3()->get_ID() - 1;
        long index4 = M->get_element(e)->get_node4()->get_ID() - 1;
        assembly_K(K, &Ks[e], index1, index2, index3, index4);
        assembly_b(b, &bs[e], index1, index2, index3, index4);
        //cout << "\t\t"; K->show(); cout << "\t\t"; b->show(); cout << "\n";
    }
}

// Aplica las condiciones de Neumann al vector b
void apply_neumann_boundary_conditions(Vector* b, Mesh* M){
    long num_conditions = M->get_quantity(NUM_NEUMANN);

    for(int c = 0; c < num_conditions; c++){
        Condition* cond = M->get_neumann_condition(c);
        
        long index = cond->get_node()->get_ID() - 1;
        b->add(cond->get_value(), index);
    }
    //cout << "\t\t"; b->show(); cout << "\n";
}

void add_column_to_RHS(Matrix* K, Vector* b, int col, float T_bar){
    for(int r = 0; r < K->get_nrows(); r++)
        b->add(-T_bar*K->get(r,col),r);
}

// Aplica las condiciones de Dirichlet a la matriz K y al vector b
void apply_dirichlet_boundary_conditions(Matrix* K, Vector* b, Mesh* M){
    long num_conditions = M->get_quantity(NUM_DIRICHLET);
    int previous_removed = 0;

    for(int c = 0; c < num_conditions; c++){
        Condition* cond = M->get_dirichlet_condition(c);
        
        long index = cond->get_node()->get_ID() - 1 - previous_removed;
        float cond_value = cond->get_value();

        //K->show();
        K->remove_row(index);
        //K->show();
        //b->show();
        b->remove_row(index);
        //b->show();

        add_column_to_RHS(K, b, index, cond_value);
        //b->show();

        K->remove_column(index);
        //K->show();

        previous_removed++;
    }
}

void solve_system_recursive(Matrix* K, Vector* b, Vector* T){
    int n = K->get_nrows();
    
    Matrix Kinv(n,n);

    cout << "\tCalculating inverse of global matrix K...\n\n";
    calculate_inverse(K, n, &Kinv);
    cout << "\tPerforming final calculation...\n\n";
    product_matrix_by_vector(&Kinv, b, n, n, T);
}

void solve_system_cholesky(Matrix* K, Vector* b, Vector* T){
    int n = K->get_nrows();
    
    Matrix Kinv(n,n);

    cout << "\tCalculating inverse of global matrix K...\n\n";
    cholesky_inverse(K, n, &Kinv);
    cout << "\tPerforming final calculation...\n\n";
    product_matrix_by_vector(&Kinv, b, n, n, T);
}

void merge_results_with_dirichlet(Vector* T, Vector* Tf, int n, Mesh* M){
    int num_dirichlet = M->get_quantity(NUM_DIRICHLET);

    int cont_dirichlet = 0;
    int cont_T = 0;
    for(int i = 0; i < n; i++){
        if(M->does_node_have_dirichlet_condition(i+1)){
            Condition* cond = M->get_dirichlet_condition(cont_dirichlet);
            cont_dirichlet++;
        
            float cond_value = cond->get_value();

            Tf->set(cond_value,i);
        }else{
            Tf->set(T->get(cont_T),i);
            cont_T++;
        }
    }
}
