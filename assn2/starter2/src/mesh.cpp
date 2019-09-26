#include "mesh.h"

#include "vertexrecorder.h"

using namespace std;

void Mesh::load( const char *filename )
{
    // 4.1. load() should populate bindVertices, currentVertices, and faces

    // Add your code here.
    ifstream meshFile(filename, ios::in);
    string s;
    float x, y, z;

    while (meshFile >> s >> x >> y >> z)
    {
        if (s == "v")
        {
            bindVertices.push_back(Vector3f(x, y, z));
        }
        if (s == "f")
        {
            faces.push_back(Tuple3u(x, y, z));
        }
    }

    // make a copy of the bind vertices as the current vertices
    currentVertices = bindVertices;
}

void Mesh::draw()
{
    // 4.2 Since these meshes don't have normals
    // be sure to generate a normal per triangle.
    // Notice that since we have per-triangle normals
    // rather than the analytical normals from
    // assignment 1, the appearance is "faceted".

    VertexRecorder meshDraw();
    for (Tuple3u triangle : faces)
    {
        // const Vector3f normal = Vector3f::cross(
        //                             Vector3f::cross(currentVertices[triangle[0] - 1], currentVertices[triangle[1]] - 1),
        //                             currentVertices[triangle[2] - 1]
        //                         ).normalized();
    }
}

void Mesh::loadAttachments( const char *filename, int numJoints )
{
    // 4.3. Implement this method to load the per-vertex attachment weights
    // this method should update m_mesh.attachments
}
