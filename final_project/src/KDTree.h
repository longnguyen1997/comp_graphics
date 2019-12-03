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
    int splitDimension = 0; // either X, Y, or Z axis
    float splitPosition; // from origin along split axis
    bool isLeaf = false;
    std::vector<Triangle *> triangles; // only leaves have lists of triangles
    BoundingBox box; // box partition for this node

    // FUNCTIONS

    bool traverse(const Ray &r, float tmin, Hit &h);
    bool traverse(const Ray &r, float tmin, Hit &h, float tstart, float tend);

    void sortTriangles(std::vector<Triangle *> &triangles,
                       int splitDimension, float splitPosition,
                       std::vector<Triangle *> &trianglesLeft,
                       std::vector<Triangle *> &trianglesRight);

    void splitBox(const BoundingBox &box, int splitDimension, float splitPosition,
                  BoundingBox &boxLeft, BoundingBox &boxRight);

    KDTree *buildTree(std::vector<Triangle *> triangles,
                      const BoundingBox &box,
                      int splitDimension
                      );

};

#endif // KDTREE_H
