#ifndef KDTREE_H
#define KDTREE_H

#include <iostream>
#include <vector>
#include <Vector3f.h>
#include "Object3D.h"
#include <limits>

using namespace std;

enum DIMSPLIT {
    x, y, z
};

class KDTree {
public:

    // CONSTRUCTOR
    KDTree() {
        dimSplit = DIMSPLIT::x;
    }

    // ATTRIBUTES
    KDTree *left, *right; // children
    int dimSplit; // either X, Y, or Z axis
    float splitDistance; // from origin along split axis
    bool isLeaf;
    vector<Triangle *> triangles; // only leaves have lists of triangles

    // FUNCTIONS
    static KDTree *buildTree(vector<Triangle *> triangles);
};

#endif // KDTREE_H
