// R-Tree Implementation

#ifndef R_TREE
#define R_TREE

#include <iostream>
#include <iomanip>

#include "node.h"
#include "rect.h"
#include <tuple>

bool VERBOSE = false;

class R_Tree {
private:
    Node* root;
    int height; // TODO
    int size;   // TODO

    Node* chooseLeaf(Node* n, Point p) {
        if(VERBOSE) {std::cout<<"chooseLeaf\n";}
        if(n->isLeaf()) {
            return n;
        }

        int container_idx = -1;
        float container_enlargement = std::numeric_limits<float>::infinity();
        float new_container_enlargement;

        for(int i = 0; i < static_cast<int>(n->entries.size()); i++) {
            adjustRect(&n->entries[i]->box, n->entries[i]->node);
            new_container_enlargement = n->entries[i]->box.enlargement(p);  // Evaluate container enlargement

            if(container_enlargement > new_container_enlargement) {     // Set container to container with least enlargement
                container_idx = i;
                container_enlargement = new_container_enlargement;
            } else if (container_enlargement == new_container_enlargement) { // Compare area for tie-breaking
                if(n->entries[container_idx]->box.area() > n->entries[i]->box.area()) {
                    container_idx = i;
                }
            }
        }

        return chooseLeaf(n->entries[container_idx]->node, p);
    }

    // Search tree for Node n's parent
    Node* findParent(Node* n) {
        if(n == root) {
            return nullptr;
        }
        return _findParent(root, n);
    }

    Node* _findParent(Node* p, Node* n) {
        for(Node::Entry* e : p->entries) {
            if(e->node == n) {
                return p;
            }
        }

        for(Node::Entry* e : p->entries) {
            Node* result = _findParent(e->node, n);
            if(result != nullptr) {
                return result;
            }
        }
        return nullptr;
    }

    // Ascend from a leaf node L to the root, adjusting covering rectangles and propagating node splits as necessary
    std::tuple<bool, Node*, Node*> adjustTree(Node* n, Node* nn) {
        if(VERBOSE) {std::cout<<"adjustTree\n";}

        // AT2. If N is root, stop and return true;
        if(n == root) {
            return std::tuple<bool, Node*, Node*>(nn != nullptr, n, nn);
        }

        // AT3. Adjust coverting rectangle in parent entry
        Node* p = findParent(n);

        if(p == nullptr) {
            n->print("no_parent\t");
        }

        Node::Entry* e_n = p->getEntry(n);

        if(e_n == nullptr) {
            p->print("no_entry_n\t");
            n->print("needs_entry\t");
        }

        adjustRect(&e_n->box, n);

        // If no split occured before function call, continue recursing
        if(nn == nullptr) {
            return adjustTree(p, nullptr);
        }

        // AT4. Propagate node split upward
        Node::Entry* e_nn = new Node::Entry;
        e_nn->node = nn;
        e_nn->box = getRect(nn);
        adjustRect(&e_nn->box, nn);

        // Add Entry e_nn to p if there's room. Otherwise, split p into p, pp
        Node* pp = nullptr;
        p->addEntry(e_nn);

        if(p->size() > p->capacity()) {
            std::cout << "adjustTree split\n";
            std::tie(p, pp) = quadraticSplit(p);

            return adjustTree(p, pp);
            //return std::tuple<bool, Node*, Node*>(p == root, p, pp);
        }
        
        // AT5. Move up to next level. Set nn = pp if split occured.
        return adjustTree(p, pp);
    }

    std::pair<Node*, Node*> quadraticSplit(Node* n) {
        if(VERBOSE) {std::cout<<"quadraticSplit\n";}
        std::pair<Node*, Node*> split_nodes;
        if(n->isLeaf()) {
            std::cout << "splitLeaf\n";
            split_nodes = splitLeaf(n);
        } else {
            std::cout << "splitBramch\n";
            split_nodes = splitBranch(n);
        }

        std::cout << "Split.first:\n";
        split_nodes.first->print();
        std::cout << "Split.second:\n";
        split_nodes.second->print();
        std::cout << "root:\n";
        root->print();
        return split_nodes;
    }

    std::pair<Node*, Node*> splitBranch(Node* n) {
        if(VERBOSE) {std::cout<<"splitBranch\n";}
        Node* nn = new Node();
        nn->is_leaf = false;

        // QS1. Pick first entry for each group
        Node::Entry* e = nullptr;
        Node::Entry* ee = nullptr;
        int e_idx, ee_idx;
        std::tie(e_idx, ee_idx) = pickSeedsBranch(n);
        e = n->entries[e_idx];
        ee = n->entries[ee_idx];

        std::vector<Node::Entry*> unassigned;
        unassigned.reserve(n->capacity() + 1);

        for(int i = 0; i < static_cast<int>(n->entries.size()); i++) {
            if(i != e_idx && i != ee_idx) {
                unassigned.push_back(n->entries[i]);
            }
        }

        n->entries.clear();

        n->entries.push_back(e);

        nn->entries.push_back(ee);

        // QS2. Check if done
        while(unassigned.size() > 0) {
            Rect n_rect = getRect(n);
            Rect nn_rect = getRect(nn);

            int next_idx = pickNextEntry(unassigned, n_rect, nn_rect);

            Node::Entry* next = unassigned[next_idx];
            unassigned.erase(unassigned.begin() + next_idx);

            // QS3. Select entry to assign
            if(n_rect.enlargementRect(next->box) > nn_rect.enlargementRect(next->box)) {
                nn->entries.push_back(next);
            } else {
                n->entries.push_back(next);
            }
        }

        return std::pair<Node*, Node*>(n, nn);
    }

    std::pair<Node*, Node*> splitLeaf(Node* n) {
        if(VERBOSE) {std::cout<<"splitLeaf\n";}
        Node* nn = new Node();

        // QS1. Pick first entry for each group
        int p_idx, pp_idx;
        std::tie(p_idx, pp_idx) = pickSeedLeaf(n);

        Point p = n->points[p_idx];
        Point pp = n->points[pp_idx];

        std::vector<Point> unassigned;
        unassigned.reserve(n->points.size());

        for(int i = 0; i < static_cast<int>(n->points.size()); i++) {
            if(i != p_idx && i != pp_idx) {
                unassigned.push_back(n->points[i]);
            }
        }

        n->points.clear();

        n->points.push_back(p);

        nn->points.push_back(pp);


        // QS2. Check if done
        while(unassigned.size() > 0) {
            Rect n_rect = getRect(n);
            Rect nn_rect = getRect(nn);

            int next_idx = pickNextPoint(unassigned, n_rect, nn_rect);

            Point next = unassigned[next_idx];
            unassigned.erase(unassigned.begin() + next_idx);

            // QS3. Select entry to assign
            if(n_rect.enlargement(next) > nn_rect.enlargement(next)) {
                nn->points.push_back(next);
            } else {
                n->points.push_back(next);
            }
        }

        return std::pair<Node*, Node*>(n, nn);
    }

    std::tuple<int, int> pickSeedsBranch(Node* n) {
        if(VERBOSE) {std::cout<<"pickSeedsBranch\n";}
        // PS1. Calculate inefficiency of grouping entries together
        int best_i, best_j;
        float best_d = 0;

        for(int i = 0; i < static_cast<int>(n->entries.size()); i++) {
            for(int j = i + 1; j < static_cast<int>(n->entries.size()); j++) {
                Point max_corner(std::max(n->entries[i]->box.getHigh().x, n->entries[j]->box.getHigh().x),
                                 std::max(n->entries[i]->box.getHigh().y, n->entries[j]->box.getHigh().y));

                Point min_corner(std::min(n->entries[i]->box.getLow().x, n->entries[j]->box.getLow().x),
                                 std::min(n->entries[i]->box.getLow().y, n->entries[j]->box.getLow().y));

                Rect J(max_corner, min_corner);

                    // PS2. Choose the most wasteful pair of rects
                    if(J.area() >= best_d) {
                        best_d = J.area();
                        best_i = i;
                        best_j = j;
                    }
            }
        }

        return std::tuple<int, int>(best_i, best_j);
    }

    std::tuple<int, int> pickSeedLeaf(Node* n) {
        if(VERBOSE) {std::cout<<"pickSeedLeaf\n";}
        // PS1. Calculate inefficiency of grouping entries together
        int best_i, best_j;
        float best_d = 0;

        for(int i = 0; i < static_cast<int>(n->points.size()); i++) {
            for(int j = i + 1; j < static_cast<int>(n->points.size()); j++) {
                Point max_corner(std::max(n->points[i].x, n->points[j].x),
                    std::max(n->points[i].y, n->points[j].y));

                Point min_corner(std::min(n->points[i].x, n->points[j].x),
                     std::min(n->points[i].y, n->points[j].y));

                Rect J(max_corner, min_corner);

                    // PS2. Choose the most wasteful pair of points
                    if(J.area() >= best_d) {
                        best_d = J.area();
                        best_i = i;
                        best_j = j;
                    }
            }
        }

        return std::tuple<int, int>(best_i, best_j);
    }

    int pickNextPoint(std::vector<Point> candidates, Rect group_1, Rect group_2) {
        if(VERBOSE) {std::cout<<"pickNextPoint\n";}
        float d1 = 0;
        float d2 = 0;
        float best_dif = -1;
        int best_option = -1;

        // PN1. Determine cost of putting each entry in each group
        for(int i = 0; i < static_cast<int>(candidates.size()); i++) {
            d1 = group_1.enlargement(candidates[i]);
            d2 = group_2.enlargement(candidates[i]);

            // PN2. Find entry with greatest preferene for one group
            if(std::abs(d1 - d2) > best_dif) {
                best_option = i;
            }
        }

        return best_option;
    }

    int pickNextEntry(std::vector<Node::Entry*> candidates, Rect group_1, Rect group_2) {
        if(VERBOSE) {std::cout<<"pickNextEntry\n";}
        float d1 = 0;
        float d2 = 0;
        float best_dif = -1;
        int best_option = -1;

        // PN1. Determine cost of putting each entry in each group
        for(int i = 0; i < static_cast<int>(candidates.size()); i++) {
            d1 = group_1.enlargementRect(candidates[i]->box);
            d2 = group_2.enlargementRect(candidates[i]->box);

            // PN2. Find entry with greatest preferene for one group
            if(std::abs(d1 - d2) > best_dif) {
                best_option = i;
            }
        }

        return best_option;
    }

    Rect getRect(Node* n) {
        if(n->isLeaf()) {
            return getRectLeaf(n->points);
        } else {
            return getRectBranch(n->entries);
        }
    }

    Rect getRectLeaf(std::vector<Point> points) {
        if(VERBOSE) {std::cout<<"getRect\n";}
        float high_x = -std::numeric_limits<float>::infinity();
        float high_y = -std::numeric_limits<float>::infinity();
        float low_x = std::numeric_limits<float>::infinity();
        float low_y = std::numeric_limits<float>::infinity();

        for(Point p : points) {
            if(p.x > high_x) {
                high_x = p.x;
            }
            if(p.x < low_x) {
                low_x = p.x;
            }
            if(p.y > high_y) {
                high_y = p.y;
            }
            if(p.y < low_y) {
                low_y = p.y;
            }
        }

        return Rect(Point(high_x, high_y), Point(low_x, low_y));
    }

    Rect getRectBranch(std::vector<Node::Entry*> entries) {
        if(VERBOSE) {std::cout<<"getRectBranch\n";}
        float high_x = -std::numeric_limits<float>::infinity();
        float high_y = -std::numeric_limits<float>::infinity();
        float low_x = std::numeric_limits<float>::infinity();
        float low_y = std::numeric_limits<float>::infinity();

        for(Node::Entry* e : entries) {
            if(e->box.getHigh().x > high_x) {
                high_x = e->box.getHigh().x;
            }
            if(e->box.getLow().x < low_x) {
                low_x = e->box.getLow().x;
            }
            if(e->box.getHigh().y > high_y) {
                high_y = e->box.getHigh().y;
            }
            if(e->box.getLow().y < low_y) {
                low_y = e->box.getLow().y;
            }
        }

        return Rect(Point(high_x, high_y), Point(low_x, low_y));
    }

    void adjustRect(Rect* r, Node* n) {
        if(VERBOSE) {std::cout<<"adjustRect\n";}
        if(n->isLeaf()) {
            for(auto& p : n->points) {
                r->updatePoint(p);
            }
        } else {
            for(auto& e : n->entries) {
                r->updateRect(e->box);
            }
        }
    }

public:
    R_Tree() {
        height = 0;
        size = 0;

        root = new Node();
    }

    ~R_Tree() {
        delete root;
    }

    void insert(Point p){
        if(VERBOSE) {std::cout<<"insert " << p.x << " " << p.y << "\n";}
        std::cout<<"insert " << p.x << " " << p.y << "\n";
        // I1. Find position for new record
        Node* l = chooseLeaf(root, p);
        Node* ll = nullptr;

        // I2. Add record to leaf node
        l->points.push_back(p);

        if(l->size() > l->capacity()) {
            std::tie(l, ll) = quadraticSplit(l);
        }

        // I3. Propagate changes upward
        bool root_split = false;
        Node* split_n = nullptr;
        Node* split_nn = nullptr;
        std::tie(root_split, split_n, split_nn) = adjustTree(l, ll);

        // I4. Grow tree taller if root splits
        if(l == root && ll != nullptr) {
            std::cout << "SPLIT 1 - ROOT IS LEAF\n";
            root = new Node();
            Rect l_r = getRect(l);
            Rect ll_r = getRect(ll);

            Node::Entry* l_e = new Node::Entry{l, l_r};
            Node::Entry* ll_e = new Node::Entry{ll, ll_r};

            root->addEntry(l_e);
            root->addEntry(ll_e);

            root->is_leaf = false;
        } else if(root_split) {
            std::cout << "SPLIT 2 - ROOT IS BRANCH\n";
            Node* root_copy = root;
            root = new Node();
            Rect root_r = getRect(root_copy);
            Rect root_split_r = getRect(split_nn);

            Node::Entry* root_e = new Node::Entry{root_copy, root_r};
            Node::Entry* root_split_e = new Node::Entry{split_nn, root_split_r};

            root->addEntry(root_e);
            root->addEntry(root_split_e);

            root->is_leaf = false;
        } else if (split_nn != nullptr) { // Non-root branch split
            std::cout << "SPILT 3 - Non-root branch split\n";
        } else if (ll != nullptr) { // Non-root leaf split
            std::cout << "SPILT 4 - Non-root leaf split\n";
        }
    }

    void print() {
        std::cout << std::setprecision(4);
        if(root != nullptr) {
            root->print();
        }
    }

    void plot() {
        if(root != nullptr) {
            root->plot();
        }
    }

    void search() { // We just need to search for the closest point
        // TODO: Search
    }

};

#endif // R_TREE