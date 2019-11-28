#ifndef KDTREE_H
#define KDTREE_H

#include <iostream>
#include <vector>
#include <Vector3f.h>
#include "Object3D.h"
#include <limits>

// FINAL PROJECT
class KDTree {
public:

    // CONSTRUCTOR

    KDTree() {};

    // ATTRIBUTES

    KDTree *left, *right; // children
    int dimSplit = 0; // either X, Y, or Z axis
    float splitDistance; // from origin along split axis
    bool isLeaf = false;
    std::vector<Triangle *> triangles; // only leaves have lists of triangles
    BoundingBox box; // box partition for this node

    // FUNCTIONS

    bool traverse(const Ray &r, float tmin, Hit &h);
    bool traverse(const Ray &r, float tmin, Hit &h, float tstart, float tend);

    void sortTriangles(std::vector<Triangle *> &triangles,
                       int dimSplit, float splitDistance,
                       std::vector<Triangle *> &trianglesLeft,
                       std::vector<Triangle *> &trianglesRight);

    void splitBox(const BoundingBox &box, int dimSplit, float splitDistance,
                  BoundingBox &boxLeft, BoundingBox &boxRight);

    KDTree *buildTree(std::vector<Triangle *> triangles,
                      const BoundingBox &box,
                      int dimSplit);

};

#endif // KDTREE_H
