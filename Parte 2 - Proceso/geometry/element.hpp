#ifndef ELEMENT_HPP
#define ELEMENT_HPP

#include "node.hpp"

class Element {
private:
    int id;
    Node* node1;
    Node* node2;
    Node* node3;

public:
    Element(int identifier, Node* first_node, Node* second_node, Node* third_node) {
        id = identifier;
        node1 = first_node;
        node2 = second_node;
        node3 = third_node;
    }

    void setId(int identifier) {
        id = identifier;
    }

    int getId() const {
        return id;
    }

    void setNode1(Node* node) {
        node1 = node;
    }

    Node* getNode1() const {
        return node1;
    }

    void setNode2(Node* node) {
        node2 = node;
    }

    Node* getNode2() const {
        return node2;
    }

    void setNode3(Node* node) {
        node3 = node;
    }

    Node* getNode3() const {
        return node3;
    }
};

#endif
