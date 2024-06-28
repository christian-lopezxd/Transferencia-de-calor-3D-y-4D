#ifndef MESH_HPP
#define MESH_HPP

#include <iostream>
#include <cstdlib>
#include "node.hpp"
#include "element.hpp"
#include "condition.hpp"
#include "BST.hpp"

enum parameter { THERMAL_CONDUCTIVITY, HEAT_SOURCE };
enum quantity { NUM_NODES, NUM_ELEMENTS, NUM_DIRICHLET, NUM_NEUMANN };

class Mesh {
private:
    float problem_data[2];
    int quantities[4];
    Tree nodes_tree;
    Element** elements;
    Condition** dirichlet_conditions;
    Condition** neumann_conditions;

public:
    Mesh() {}

    ~Mesh() {
        free(elements);
        free(dirichlet_conditions);
        free(neumann_conditions);
    }

    void setProblemData(float k, float Q) {
        problem_data[THERMAL_CONDUCTIVITY] = k;
        problem_data[HEAT_SOURCE] = Q;
    }

    float getProblemData(parameter pos) const {
        return problem_data[pos];
    }

    void setQuantities(int num_nodes, int num_elements, int num_dirichlet, int num_neumann) {
        quantities[NUM_NODES] = num_nodes;
        quantities[NUM_ELEMENTS] = num_elements;
        quantities[NUM_DIRICHLET] = num_dirichlet;
        quantities[NUM_NEUMANN] = num_neumann;
    }

    int getQuantity(quantity pos) const {
        return quantities[pos];
    }

    void initArrays() {
        elements = (Element**)malloc(sizeof(Element*) * quantities[NUM_ELEMENTS]);
        dirichlet_conditions = (Condition**)malloc(sizeof(Condition*) * quantities[NUM_DIRICHLET]);
        neumann_conditions = (Condition**)malloc(sizeof(Condition*) * quantities[NUM_NEUMANN]);
    }

   void insertNode(int id, double x, double y) {
    cout << "Inserting node " << id << " at coordinates (" << x << ", " << y << ")" << endl;
    nodes_tree.insert(id, x, y);
}


    Node* getNode(int id) {
        return nodes_tree.find(id);
    }

    void insertElement(Element* element, int pos) {
        elements[pos] = element;
    }

    Element* getElement(int pos) {
        return elements[pos];
    }

    void insertDirichletCondition(Condition* condition, int pos) {
        dirichlet_conditions[pos] = condition;
    }

    Condition* getDirichletCondition(int pos) const {
        return dirichlet_conditions[pos];
    }

    bool doesNodeHaveDirichletCondition(int id) const {
        for (int i = 0; i < quantities[NUM_DIRICHLET]; i++) {
            if (dirichlet_conditions[i]->getNode()->id == id) {
                return true;
            }
        }
        return false;
    }

    void insertNeumannCondition(Condition* condition, int pos) {
        neumann_conditions[pos] = condition;
    }

    Condition* getNeumannCondition(int pos) const {
        return neumann_conditions[pos];
    }

    void report() {
        cout << "Problem Data\n**********************\n";
        cout << "Thermal Conductivity: " << problem_data[THERMAL_CONDUCTIVITY] << "\n";
        cout << "Heat Source: " << problem_data[HEAT_SOURCE] << "\n\n";
        cout << "Quantities\n***********************\n";
        cout << "Number of nodes: " << quantities[NUM_NODES] << "\n";
        cout << "Number of elements: " << quantities[NUM_ELEMENTS] << "\n";
        cout << "Number of Dirichlet boundary conditions: " << quantities[NUM_DIRICHLET] << "\n";
        cout << "Number of Neumann boundary conditions: " << quantities[NUM_NEUMANN] << "\n\n";
        cout << "List of nodes\n**********************\n";
        nodes_tree.show(IN);
        cout << "\nList of elements\n**********************\n";
        for (int i = 0; i < quantities[NUM_ELEMENTS]; i++) {
            cout << "Element: " << elements[i]->getId() << ", Node 1= " << elements[i]->getNode1()->id;
            cout << ", Node 2= " << elements[i]->getNode2()->id << ", Node 3= " << elements[i]->getNode3()->id << "\n";
        }
        cout << "\nList of Dirichlet boundary conditions\n**********************\n";
        for (int i = 0; i < quantities[NUM_DIRICHLET]; i++) {
            cout << "Condition " << i + 1 << ": " << dirichlet_conditions[i]->getNode()->id << ", Value= " << dirichlet_conditions[i]->getValue() << "\n";
        }
        cout << "\nList of Neumann boundary conditions\n**********************\n";
        for (int i = 0; i < quantities[NUM_NEUMANN]; i++) {
            cout << "Condition " << i + 1 << ": " << neumann_conditions[i]->getNode()->id << ", Value= " << neumann_conditions[i]->getValue() << "\n";
        }
        cout << "\n";
    }
};

#endif
