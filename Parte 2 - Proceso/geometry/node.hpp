#ifndef NODE_HPP
#define NODE_HPP

struct Node {
    int id;
    double x_coordinate, y_coordinate;
    Node* left;
    Node* right;
    Node* parent;

    Node(int identifier, double coordX, double coordY)
        : id(identifier), x_coordinate(coordX), y_coordinate(coordY), left(nullptr), right(nullptr), parent(nullptr) {}
};

#endif
