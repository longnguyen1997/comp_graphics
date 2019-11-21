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
    KDTree();

    // ATTRIBUTES
    KDTree *backNode, frontNode; // children
    int dimSplit; // either X, Y, or Z axis
    float splitDistance; // from origin along split axis
    bool isLeaf;
    vector<Object3D> triangles; // only leaves have lists of triangles

    // FUNCTIONS
    
};

#endif // KDTREE_H
