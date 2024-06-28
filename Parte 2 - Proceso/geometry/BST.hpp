#ifndef BST_HPP
#define BST_HPP

#include <iostream>
#include <cstdlib>
#include "node.hpp"

using namespace std;

enum order { IN, PRE, POST };

class Tree {
private:
    Node* root;

    Node* createNode(int id, double x, double y) {
        return new Node(id, x, y);
    }

    void insertAux(Node* currentNode, int id, double x, double y) {
        if (id <= currentNode->id) {
            if (currentNode->left == nullptr) {
                currentNode->left = createNode(id, x, y);
                currentNode->left->parent = currentNode;
            } else {
                insertAux(currentNode->left, id, x, y);
            }
        } else {
            if (currentNode->right == nullptr) {
                currentNode->right = createNode(id, x, y);
                currentNode->right->parent = currentNode;
            } else {
                insertAux(currentNode->right, id, x, y);
            }
        }
    }

    void showPreOrder(Node* currentNode) {
        if (currentNode == nullptr) {
            return;
        }
        cout << "[" << currentNode->id << " (" << currentNode->x_coordinate << "," << currentNode->y_coordinate << ")] ";
        showPreOrder(currentNode->left);
        showPreOrder(currentNode->right);
    }

    void showInOrder(Node* currentNode) {
        if (currentNode == nullptr) {
            return;
        }
        showInOrder(currentNode->left);
        cout << "[" << currentNode->id << " (" << currentNode->x_coordinate << "," << currentNode->y_coordinate << ")] ";
        showInOrder(currentNode->right);
    }

    void showPostOrder(Node* currentNode) {
        if (currentNode == nullptr) {
            return;
        }
        showPostOrder(currentNode->left);
        showPostOrder(currentNode->right);
        cout << "[" << currentNode->id << " (" << currentNode->x_coordinate << "," << currentNode->y_coordinate << ")] ";
    }

    Node* findAux(Node* currentNode, int id) {
        if (currentNode == nullptr || currentNode->id == id) {
            return currentNode;
        }
        if (id < currentNode->id) {
            return findAux(currentNode->left, id);
        } else {
            return findAux(currentNode->right, id);
        }
    }

public:
    Tree() : root(nullptr) {}

    void insert(int id, double x, double y) {
        if (root == nullptr) {
            root = createNode(id, x, y);
        } else {
            insertAux(root, id, x, y);
        }
    }

    void show(order mode) {
        switch (mode) {
            case IN:
                showInOrder(root);
                break;
            case PRE:
                showPreOrder(root);
                break;
            case POST:
                showPostOrder(root);
                break;
        }
        cout << endl;
    }

    Node* find(int id) {
        return findAux(root, id);
    }
};

#endif
