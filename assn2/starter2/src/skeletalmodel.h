#ifndef SKELETALMODEL_H
#define SKELETALMODEL_H

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <sstream>

#include <vecmath.h>

#include "gl.h"
#include "tuple.h"
#include "joint.h"
#include "mesh.h"
#include "matrixstack.h"
#include "camera.h"

class SkeletalModel
{
public:
    SkeletalModel();
    ~SkeletalModel();
    // Already-implemented utility functions that call the code you will write.
    void load(const char *skeletonFile, const char *meshFile, const char *attachmentsFile);
    void draw(const Camera& camera, bool drawSkeleton);
    void updateShadingUniforms();

    // Part 1: Understanding Hierarchical Modeling

    // 1.1. Implement method to load a skeleton.
    // This method should compute m_rootJoint and populate m_joints.
    void loadSkeleton(const char* filename);

    // 1.1. Implement this method with a recursive helper to draw a sphere at each joint.
    void drawJoints(const Camera& camera);
    // Helper function for 1.1.
    void traverseAndDrawJoint(const Camera& camera, const Joint* joint);

    // 1.2. Implement this method a recursive helper to draw a box between each pair of joints
    void drawSkeleton(const Camera& camera);

    // 1.3. Implement this method to handle changes to your skeleton given
    // changes in the slider values
    void setJointTransform(int jointIndex, float rX, float rY, float rZ);

    // Part 2: Skeletal Subspace Deformation

    // 2.3. Implement SSD

    // 2.3.1. Implement this method to compute a per-joint transform from
    // world-space to joint space in the BIND POSE.
    void computeBindWorldToJointTransforms();

    // 2.3.2. Implement this method to compute a per-joint transform from
    // joint space to world space in the CURRENT POSE.
    void updateCurrentJointToWorldTransforms();

    // 2.3.2. This is the core of SSD.
    // Implement this method to update the vertices of the mesh
    // given the current state of the skeleton.
    // You will need both the bind pose world --> joint transforms.
    // and the current joint --> world transforms.
    void updateMesh();

private:
    // pointer to the root joint
    Joint* m_rootJoint;
    // the list of joints.
    std::vector< Joint* > m_joints;
    Mesh m_mesh;
    MatrixStack m_matrixStack;
    GLuint program;
};

#endif
