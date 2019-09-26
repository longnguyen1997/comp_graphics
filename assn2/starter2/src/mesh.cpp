#include "mesh.h"

#include "vertexrecorder.h"
#include <cassert>

using namespace std;

void Mesh::load( const char *filename )
{
    // 4.1. load() should populate bindVertices, currentVertices, and faces

    // Add your code here.
    ifstream meshFile(filename, ios::in);
    char s;
    float x, y, z;
    while (meshFile >> s >> x >> y >> z)
    {
        if (s == 'v')
        {
            bindVertices.push_back(Vector3f(x, y, z));
        }
        if (s == 'f')
        {
            // Faces are 1-indexed, so subtract 1.
            faces.push_back(Tuple3u((int)x - 1, (int)y - 1, (int)z - 1));
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

    VertexRecorder meshDraw;
    for (Tuple3u triangle : faces)
    {
        const Vector3f A = currentVertices[triangle[0]];
        const Vector3f B = currentVertices[triangle[1]];
        const Vector3f C = currentVertices[triangle[2]];
        // N = (B - A) x (C - A); https://stackoverflow.com/a/1966605.
        const Vector3f normal = Vector3f::cross(
                                    B - A, C - A
                                ).normalized();
        meshDraw.record(A, normal);
        meshDraw.record(B, normal);
        meshDraw.record(C, normal);
    }
    cout << "Loaded vertices and faces for the model mesh." << endl;
    meshDraw.draw();
}

void Mesh::loadAttachments( const char *filename, int numJoints )
{
    // 4.3. Implement this method to load the per-vertex attachment weights
    // this method should update m_mesh.attachments

    ifstream attachmentsFile(filename, ios::in);
    vector<float> weights(numJoints - 1, 0.0f);
    while (true)
    {
        for (int j = 0; j < numJoints - 1; ++j)
        {
            float w;
            // EOF, return.
            if (!(attachmentsFile >> w))
            {
                cout << "Loaded attachment weights." << endl;
                return;
            }
            weights[j] = w;
        }
        attachments.push_back(weights);
    }
}
