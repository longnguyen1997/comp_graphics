#include "Object3D.h"
#include "KDTree.h"
#include <limits>
#include <iostream>

bool PRINT_DEBUG = true;

bool KDTree::traverse(const Ray &r, float tmin, Hit &h)
{
    vector<float> pathTimes = box.intersect(r);
    if (pathTimes.empty())
    {
        // cout << "No intersection with bounding box" << endl;
        return false; // looks good - not a bug unless ray-box intersection is wrong
    }
    float tstart = pathTimes[0];
    float tend = pathTimes[1];
    if (tstart > tend)
        throw - 1;
    return traverse(r, tmin, h, tstart, tend);
}

bool KDTree::traverse(const Ray &r, float tmin, Hit &h, float tstart, float tend)
{
    assert(tstart <= tend);
    if (isLeaf)
    {
        // leaf code here should be correct, don't touch
        bool result = false;
        for (Triangle *triangle : triangles)
        {
            if (triangle->intersect(r, tmin, h))
            {
                // cout << "   > INTERSECTION FOUND AT LEAF NODE" << endl;
                result = true;
            }
        }
        return result;
    }
    else
    {
        // Find the intersection with the split axis
        Vector3f orig = r.getOrigin();
        Vector3f dir = r.getDirection();
        // P = dt + O -> t = (P - O) / d
        float t = (splitPosition - orig[dimSplit]) / (dir[dimSplit]);

        KDTree *front, *back;
        front = left;
        back = right;
        // TODO: Account for transformations. Not in this implementation.
        // int belowFirst = (orig[dimSplit] < splitPosition) ||
        //                  (orig[dimSplit] == splitPosition && dir[dimSplit] <= 0);
        // if (not belowFirst)
        //     swap(front, back);
        if (dir[dimSplit] < 0) swap(front, back);

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
            bool A = false;
            bool B = false;
            A = front->traverse(r, tmin, h, tstart, t);
            B = back->traverse(r, tmin, h, t, tend);
            return A or B;
        }
    }
}

void KDTree::splitBox(const BoundingBox &box, int dimSplit, float axisSplitPosition,
                      BoundingBox &boxLeft, BoundingBox &boxRight)
{
    boxLeft = BoundingBox(box.minBounds(), box.maxBounds());
    boxRight = BoundingBox(box.minBounds(), box.maxBounds());
    boxLeft.max[dimSplit] = axisSplitPosition;
    boxRight.min[dimSplit] = axisSplitPosition;
    assert(boxLeft.max[dimSplit] <= boxRight.min[dimSplit]);
}

// this is definitely correct, don't touch
void KDTree::sortTriangles(vector<Triangle *> &triangles,
                           int dimSplit, float axisSplitPosition,
                           vector<Triangle *> &trianglesLeft,
                           vector<Triangle *> &trianglesRight)
{
    for (Triangle *t : triangles)
    {
        if (t->box.min[dimSplit] <= axisSplitPosition)
        {
            trianglesLeft.push_back(t);
        }
        if (t->box.max[dimSplit] >= axisSplitPosition)
        {
            trianglesRight.push_back(t);
        }
    }
}

KDTree *KDTree::buildTree(std::vector<Triangle *> triangles,
                          const BoundingBox &box,
                          int dimSplit, int numLeafTris)
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
    // Naively split at half of the current distance.
    // TODO: Implement sliding midpoint.
    assert(box.min[dimSplit] < box.max[dimSplit]);
    float axisSplitPosition = box.min[dimSplit] + (box.d(dimSplit) / 2.f); // midpoint method
    BoundingBox boxLeft, boxRight;
    splitBox(box,
             dimSplit,
             axisSplitPosition,
             boxLeft,
             boxRight);

    std::vector<Triangle *> trianglesLeft, trianglesRight;
    sortTriangles(triangles,
                  dimSplit,
                  axisSplitPosition,
                  trianglesLeft,
                  trianglesRight);

    int nextDimension = (dimSplit + 1) % 3;
    KDTree *root = new KDTree();
    root->dimSplit = dimSplit;
    root->splitPosition = axisSplitPosition;
    root->box = box;
    root->left = buildTree(trianglesLeft, boxLeft, nextDimension, numLeafTris);
    root->right = buildTree(trianglesRight, boxRight, nextDimension, numLeafTris);
    return root;
}