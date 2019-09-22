#include "mesh.h"

#include "vertexrecorder.h"

using namespace std;

void Mesh::load( const char* filename )
{
	// 4.1. load() should populate bindVertices, currentVertices, and faces

	// Add your code here.

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
}

void Mesh::loadAttachments( const char* filename, int numJoints )
{
	// 4.3. Implement this method to load the per-vertex attachment weights
	// this method should update m_mesh.attachments
}
