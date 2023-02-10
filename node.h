#ifndef NODE_H
#define NODE_H

#include "rect.h"
#include "point.h"
#include <vector>
#include <limits>

class Node
{
    friend class R_Tree;

    struct Entry {
        Node* node;
        Rect box;
    };
    
    public:
    std::vector<Entry*> entries;
    std::vector<Point> points;
    bool is_leaf;
    const int MAX_CAPACITY = 5; // Max # of children a node may have

    Node() : is_leaf(true) {
        entries.reserve(MAX_CAPACITY+1);
        points.reserve(MAX_CAPACITY+1);
    }

    ~Node() {
        // std::cout << "freeing node\n";
        // std::cout << "\tis_leaf:" << is_leaf << "\n";
        // std::cout << "\tparent == nullptr (root): " << (parent == nullptr) << "\n";
        // std::cout << "\tentries.size(): " << entries.size() << "\n";
        // std::cout << "\tpoints.size(): " << points.size() << "\n";

        for(int i = 0; i < static_cast<int>(entries.size()); i++){
            delete entries[i]->node;
            delete entries[i];
            entries[i] = nullptr;
        }
    }

    bool isLeaf() {
        return is_leaf;
    }

    int size() {
        if(isLeaf()) {
            return points.size();
        } else {
            return entries.size();
        }
    }

    int capacity() {
        return MAX_CAPACITY;
    }

    // Return whether or not a node is an entry
    bool contains(Node* n) {
        return getEntry(n) != nullptr;
    }

    // Return the entry struct for a given node
    // Returns nullptr if node is not an entry
    Entry* getEntry(Node* n) {
        for(int i = 0; i < static_cast<int>(entries.size()); i++) {
            if(entries[i]->node == n) {
                return entries[i];
            }
        }
        return nullptr;
    }

    void addEntry(Entry* e) {
        entries.push_back(e);
    }

    void print(std::string indent = "") {
        if(isLeaf()) {
            std::cout << indent << "LEAF\n";
            for(Point p : points) {
                std::cout << indent << "Point\t" << p.x << "\t" << p.y << "\n";
            }
        } else {
            int i = 1;
            std::cout << indent << "BRANCH\n";
            for(Entry* e : entries) {
                std::string e_is_leaf = e->node->isLeaf() ? "leaf" : "branch";
                std::cout << indent << "Entry " << i << "\t" << e_is_leaf << "\n";
                e->node->print(indent + "\t");
                i++;
            }
        }
    }

};

#endif //NODE_H