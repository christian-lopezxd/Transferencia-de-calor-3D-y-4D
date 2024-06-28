#ifndef CONDITION_HPP
#define CONDITION_HPP

#include "node.hpp"

class Condition {
private:
    Node* node;
    float value;

public:
    Condition(Node* node_to_assign, float value_to_assign) {
        node = node_to_assign;
        value = value_to_assign;
    }

    void setNode(Node* node_to_assign) {
        node = node_to_assign;
    }

    Node* getNode() {
        return node;
    }

    void setValue(float value_to_assign) {
        value = value_to_assign;
    }

    float getValue() {
        return value;
    }
};

#endif
