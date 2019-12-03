#include "Object3D.h"
#include "KDTree.h"
#include <limits>
#include <algorithm>
#include <experimental/algorithm>
#include <iostream>
#include <random>
#include <vector>

bool PRINT_DEBUG = true;

bool KDTree::traverse(const Ray &r, float tmin, Hit &h)
{
    vector<float> pathTimes = box.intersect(r);
    if (pathTimes.empty())
    {
        return false;
    }
    float tstart = pathTimes[0];
    float tend = pathTimes[1];
    return traverse(r, tmin, h, tstart, tend);
}

bool KDTree::traverse(const Ray &r, float tmin, Hit &h, float tstart, float tend)
{
    assert(tstart <= tend);
    if (isLeaf)
    {
        bool result = false;
        for (Triangle *triangle : triangles)
        {
            result |= triangle->intersect(r, tmin, h);
        }
        return result;
    }
    else
    {
        // Find the intersection with the split axis
        Vector3f orig = r.getOrigin();
        Vector3f dir = r.getDirection();
        // P = dt + O -> t = (P - O) / d
        float t = (splitPosition - orig[splitDimension]) / (dir[splitDimension]);

        KDTree *front, *back;
        front = left;
        back = right;
        int belowFirst = (orig[splitDimension] < splitPosition) ||
                         (orig[splitDimension] == splitPosition && dir[splitDimension] <= 0);
        if (not belowFirst)
            swap(front, back);

        // 3 cases to check for
        if (t <= tstart)
        {
            return back->traverse(r, tmin, h, tstart, tend);
        }
        else if (t >= tend or t < 0)
        {
            return front->traverse(r, tmin, h, tstart, tend);
        }
        else
        {
            if (front->traverse(r, tmin, h, tstart, t) and h.getT() < t)
                /*
                If front region already contains something,
                terminate the search.
                */
                return true;
            return back->traverse(r, tmin, h, t, tend);
        }
    }
}

void KDTree::splitBox(const BoundingBox &box, int splitDimension, float splitPosition,
                      BoundingBox &boxLeft, BoundingBox &boxRight)
{
    boxLeft = BoundingBox(box.minBounds(), box.maxBounds());
    boxRight = BoundingBox(box.minBounds(), box.maxBounds());
    boxLeft.max[splitDimension] = splitPosition;
    boxRight.min[splitDimension] = splitPosition;
    assert(boxLeft.max[splitDimension] <= boxRight.min[splitDimension]);
}

void KDTree::sortTriangles(vector<Triangle *> &triangles,
                           int splitDimension, float splitPosition,
                           vector<Triangle *> &trianglesLeft,
                           vector<Triangle *> &trianglesRight)
{
    for (Triangle *t : triangles)
    {
        if (t->box.min[splitDimension] <= splitPosition)
        {
            trianglesLeft.push_back(t);
        }
        if (t->box.max[splitDimension] >= splitPosition)
        {
            trianglesRight.push_back(t);
        }
    }
}

KDTree *KDTree::buildTree(std::vector<Triangle *> triangles,
                          const BoundingBox &box,
                          int splitDimension)
{
    if (!PRINT_DEBUG)
        cout.rdbuf(nullptr);

    // Base case.
    if (triangles.size() <= 11)
    {
        // cout << "Leaf node, " << triangles.size() << endl;
        KDTree *leaf = new KDTree();
        leaf->isLeaf = true;
        leaf->box = box;
        leaf->triangles = triangles;
        return leaf;
    }

    // Recursive case: build subtrees.

    assert(box.min[splitDimension] < box.max[splitDimension]); // segfault if not true!

    // SPLITTING ALGORITHM: MIDPOINT
    // Naively split at half of the current distance.
    float splitPosition = box.min[splitDimension] + (box.d(splitDimension) / 2.f);
    std::vector<Triangle *> trianglesLeft, trianglesRight;
    sortTriangles(triangles,
                  splitDimension,
                  splitPosition,
                  trianglesLeft,
                  trianglesRight);
    BoundingBox boxLeft, boxRight;
    splitBox(box,
             splitDimension,
             splitPosition,
             boxLeft,
             boxRight);

    // Cycle through dimensions.
    int nextDimension = (splitDimension + 1) % 3;
    KDTree *root = new KDTree();
    root->splitDimension = splitDimension;
    root->splitPosition = splitPosition;
    root->box = box;
    root->left = buildTree(trianglesLeft, boxLeft, nextDimension);
    root->right = buildTree(trianglesRight, boxRight, nextDimension);
    return root;
}