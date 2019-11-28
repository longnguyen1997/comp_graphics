#include "Object3D.h"
#include "KDTree.h"
#include <limits>
#include <iostream>

bool PRINT_DEBUG = true;

bool KDTree::traverse(const Ray &r, float tmin, Hit &h) {
    vector<float> pathTimes = box.intersect(r);
    if (pathTimes.size() == 0) return false;
    return traverse(r, tmin, h, pathTimes[0], pathTimes[1]);
}

bool KDTree::traverse(const Ray &r, float tmin, Hit &h, float tstart, float tend) {
    if (isLeaf) {
        for (Triangle *triangle : triangles) {
            if (triangle->intersect(r, tmin, h)) return true;
        }
    } else {
        float inverseDirAxis = 1.f / r.getDirection()[dimSplit];
        float t = (splitDistance - r.getOrigin()[dimSplit]) * (r.getDirection()[dimSplit] == 0 ? INFINITY : inverseDirAxis);
        if(t < tmin) return false;

        // near is the side containing the origin of the ray
        KDTree *near, *far;
        near = right;
        far = left;
        if(r.getOrigin()[dimSplit] < splitDistance) swap(near, far);

        // 3 cases to check for
        if( t >= tend || t < 0) {
            return near->traverse(r, tmin, h, tstart, tend);
        } else if (t <= tstart) {
            return far->traverse(r, tmin, h, tstart, tend);
        } else {
            return (near->traverse(r, tmin, h, tstart, t) ||
                    far->traverse(r, tmin, h, t, tend));
        }
    }
    return false;
}

void KDTree::splitBox(const BoundingBox &box, int dimSplit, float splitDistance,
                      BoundingBox &boxLeft, BoundingBox &boxRight) {
    // cout << "splitting bounding box" << endl;
    boxLeft = box;
    boxRight = box;
    float displacement = boxLeft.min[dimSplit] + splitDistance;
    boxLeft.max[dimSplit] = displacement;
    boxRight.min[dimSplit] = displacement;
}

void KDTree::sortTriangles(vector<Triangle *> &triangles,
                           int dimSplit, float splitDistance,
                           vector<Triangle *> &trianglesLeft,
                           vector<Triangle *> &trianglesRight) {
    // Sort triangles into left and right buckets depending on
    for (int i = 0; i < triangles.size(); ++i) {
        Triangle *t = triangles[i];
        BoundingBox tBox = t->box;
        // If it crosses both boundaries, add it to both.
        if (tBox.min[dimSplit] <= splitDistance) trianglesLeft.push_back(t);
        if (tBox.max[dimSplit] >= splitDistance) trianglesRight.push_back(t);
    }
}

int c = 0;
KDTree *KDTree::buildTree(std::vector<Triangle *> triangles,
                          const BoundingBox &box,
                          int dimSplit) {

    if (!PRINT_DEBUG) cout.rdbuf(nullptr);

    c++;
    cout << endl << "Function call #" << c << endl;
    switch(dimSplit) {
    case 0:
        cout << "   > X AXIS: " << triangles.size() << " triangles, box from " << box.min[dimSplit] << " to " << box.max[dimSplit] << endl;
        break;
    case 1:
        cout << "   > Y AXIS: " << triangles.size() << " triangles, box from " << box.min[dimSplit] << " to " << box.max[dimSplit] << endl;
        break;
    case 2:
        cout << "   > Z AXIS: " << triangles.size() << " triangles, box from " << box.min[dimSplit] << " to " << box.max[dimSplit] << endl;
        break;
    }

    // Base case.
    if (triangles.size() <= 15) {
        cout << "   > Building leaf node with " << triangles.size() << " triangles..." << endl;
        KDTree *leaf = new KDTree();
        leaf->isLeaf = true;
        leaf->box = box;
        leaf->triangles = triangles;
        return leaf;
    }

    // Recursive case: build subtrees.
    // Naively split at half of the current distance.
    // TODO: Implement sliding midpoint for efficiency.
    float splitDistance = box.d(dimSplit) / 2.f; // midpoint method
    BoundingBox boxLeft, boxRight;
    splitBox(box, dimSplit, splitDistance,
             boxLeft, boxRight);

    cout << "   > Left box split: from " << boxLeft.min[dimSplit] << " to " << boxLeft.max[dimSplit] << endl;
    cout << "   > Right box split: from " << boxRight.min[dimSplit] << " to " << boxRight.max[dimSplit] << endl;

    std::vector<Triangle *> trianglesLeft, trianglesRight;
    sortTriangles(triangles, dimSplit, box.min[dimSplit] + splitDistance,
                  trianglesLeft, trianglesRight);
    KDTree *root = new KDTree();
    int nextDimension = (dimSplit + 1) % 3;
    root->dimSplit = dimSplit;
    root->box = box;
    root->left = buildTree(trianglesLeft, boxLeft, nextDimension);
    root->right = buildTree(trianglesRight, boxRight, nextDimension);
    return root;
}