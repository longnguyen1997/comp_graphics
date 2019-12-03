#include "Mesh.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <utility>
#include <sstream>
#include "KDTree.h"

Mesh::Mesh(const std::string &filename, Material *material) : Object3D(material)
{
    isMesh = true;
    std::ifstream f;
    f.open(filename.c_str());
    if (!f.is_open())
    {
        std::cout << "Cannot open " << filename << "\n";
        return;
    }
    // cout << "mesh file " << filename.c_str() << endl;

    std::vector<Vector3f> v;
    std::vector<ObjTriangle> t;
    std::vector<Vector3f> n;
    std::vector<Vector2f> texCoord;

    const std::string vTok("v");
    const std::string fTok("f");
    const std::string texTok("vt");
    const char bslash = '/', space = ' ';
    std::string tok;
    std::string line;
    while (true)
    {
        std::getline(f, line);
        if (f.eof())
        {
            break;
        }
        if (line.size() < 3)
        {
            continue;
        }
        if (line.at(0) == '#')
        {
            continue;
        }
        std::stringstream ss(line);
        ss >> tok;
        if (tok == vTok)
        {
            Vector3f vec;
            ss >> vec[0] >> vec[1] >> vec[2];
            v.push_back(vec);
        }
        else if (tok == fTok)
        {
            if (line.find(bslash) != std::string::npos)
            {
                std::replace(line.begin(), line.end(), bslash, space);
                std::stringstream facess(line);
                ObjTriangle trig;
                facess >> tok;
                for (int ii = 0; ii < 3; ii++)
                {
                    facess >> trig[ii] >> trig.texID[ii];
                    trig[ii]--;
                    trig.texID[ii]--;
                }
                t.push_back(trig);
            }
            else
            {
                ObjTriangle trig;
                for (int ii = 0; ii < 3; ii++)
                {
                    ss >> trig[ii];
                    trig[ii]--;
                    trig.texID[ii] = 0;
                }
                t.push_back(trig);
            }
        }
        else if (tok == texTok)
        {
            Vector2f texcoord;
            ss >> texcoord[0];
            ss >> texcoord[1];
            texCoord.push_back(texcoord);
        }
    }
    f.close();

    // Compute normals
    // will smooth normals.
    // if sharp edges required, build OBJ with no shared vertices.
    n.resize(v.size());
    for (int ii = 0; ii < t.size(); ii++)
    {
        Vector3f a = v[t[ii][1]] - v[t[ii][0]];
        Vector3f b = v[t[ii][2]] - v[t[ii][0]];
        Vector3f normal = Vector3f::cross(a, b).normalized();
        for (int jj = 0; jj < 3; jj++)
        {
            n[t[ii][jj]] += normal;
        }
    }
    for (int ii = 0; ii < v.size(); ii++)
    {
        n[ii] = n[ii] / n[ii].abs();
    }

    // Set up triangles
    for (int i = 0; i < t.size(); i++)
    {
        Triangle triangle(v[t[i][0]],
                          v[t[i][1]],
                          v[t[i][2]],
                          n[t[i][0]],
                          n[t[i][1]],
                          n[t[i][2]],
                          getMaterial());
        _triangles.push_back(triangle);
    }

    cout << filename << " mesh size: " << _triangles.size() << endl;

    // FINAL PROJECT
    // Calculate bounding box of triangles
    // Get the global extrema.
    Vector3f minBounds(INFINITY, INFINITY, INFINITY);
    Vector3f maxBounds(-INFINITY, -INFINITY, -INFINITY);
    std::vector<Triangle *> triangles;
    for (Triangle &t : _triangles)
    {
        triangles.push_back(&t);
        // Slide 43 of L12 - Accelerating Raytracing.
        minBounds.x() = min(minBounds.x(), (t.box).min.x());
        minBounds.y() = min(minBounds.y(), (t.box).min.y());
        minBounds.z() = min(minBounds.z(), (t.box).min.z());
        maxBounds.x() = max(maxBounds.x(), (t.box).max.x());
        maxBounds.y() = max(maxBounds.y(), (t.box).max.y());
        maxBounds.z() = max(maxBounds.z(), (t.box).max.z());
    }
    box = BoundingBox(minBounds, maxBounds);
    this->triangles = triangles;

    // Start on x axis.
    int splitDimension = 0;
    this->rootKD = this->rootKD->buildTree(triangles,
                               box,
                               splitDimension
                               );
    checkTrianglesInKDTree();

    octree.build(this);
}

bool checkTriangle(Triangle *t, KDTree *node)
{
    if (node->isLeaf)
    {
        for (Triangle *tNode : node->triangles)
        {
            if (tNode == t)
                return true;
        }
        return false;
    }
    bool A = false;
    bool B = false;
    if (t->box.min[node->splitDimension] <= node->splitPosition)
    {
        A = checkTriangle(t, node->left);
    }
    if (t->box.max[node->splitDimension] >= node->splitPosition)
    {
        B = checkTriangle(t, node->right);
    }
    return A or B;
}

bool Mesh::checkTrianglesInKDTree()
{
    // Verifies that all triangles in this mesh
    // are actually in the constructed KD tree.
    for (Triangle *t : triangles)
    {
        if (!checkTriangle(t, rootKD))
            throw - 1;
    }
    return true;
}

bool Mesh::intersect(const Ray &r, float tmin, Hit &h) const
{
#if 0
    ray = &r;
    hit = &h;
    tm = tmin;
    return octree.intersect(r);
#else

    // FINAL PROJECT: Smarter traversal
    return rootKD->traverse(r, tmin, h);

    // Naive traversal across all triangles
    // bool result = false;
    // for (Triangle t : _triangles) {
    //     if (t.intersect(r, tmin, h)) {
    //         result = true;
    //     }
    // }
    // return result;
#endif
}

bool Mesh::intersectTrig(int idx, const Ray &r) const
{
    const Triangle &triangle = _triangles[idx];
    bool result = triangle.intersect(r, tm, *hit);
    return result;
}
