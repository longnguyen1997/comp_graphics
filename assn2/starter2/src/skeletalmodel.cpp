#include "skeletalmodel.h"
#include <cassert>

#include "starter2_util.h"
#include "vertexrecorder.h"
#include <iostream>
#include <map>

using namespace std;

SkeletalModel::SkeletalModel()
{
    program = compileProgram(c_vertexshader, c_fragmentshader_light);
    if (!program)
    {
        printf("Cannot compile program\n");
        assert(false);
    }
}

SkeletalModel::~SkeletalModel()
{
    // destructor will release memory when SkeletalModel is deleted
    while (m_joints.size())
    {
        delete m_joints.back();
        m_joints.pop_back();
    }

    glDeleteProgram(program);
}

void SkeletalModel::load(const char *skeletonFile, const char *meshFile, const char *attachmentsFile)
{
    loadSkeleton(skeletonFile);

    m_mesh.load(meshFile);
    m_mesh.loadAttachments(attachmentsFile, (int)m_joints.size());

    computeBindWorldToJointTransforms();
    updateCurrentJointToWorldTransforms();
}

void SkeletalModel::draw(const Camera &camera, bool skeletonVisible)
{
    // draw() gets called whenever a redraw is required
    // (after an update() occurs, when the camera moves, the window is resized, etc)

    m_matrixStack.clear();

    glUseProgram(program);
    updateShadingUniforms();
    if (skeletonVisible)
    {
        drawJoints(camera);
        drawSkeleton(camera);
    }
    else
    {
        // Tell the mesh to draw itself.
        // Since we transform mesh vertices on the CPU,
        // There is no need to set a Model matrix as uniform
        camera.SetUniforms(program, Matrix4f::identity());
        m_mesh.draw();
    }
    glUseProgram(0);
}

void SkeletalModel::updateShadingUniforms()
{
    // UPDATE MATERIAL UNIFORMS
    GLfloat diffColor[] = { 0.4f, 0.4f, 0.4f, 1 };
    GLfloat specColor[] = { 0.9f, 0.9f, 0.9f, 1 };
    GLfloat shininess[] = { 50.0f };
    int loc = glGetUniformLocation(program, "diffColor");
    glUniform4fv(loc, 1, diffColor);
    loc = glGetUniformLocation(program, "specColor");
    glUniform4fv(loc, 1, specColor);
    loc = glGetUniformLocation(program, "shininess");
    glUniform1f(loc, shininess[0]);

    // UPDATE LIGHT UNIFORMS
    GLfloat lightPos[] = { 3.0f, 3.0f, 5.0f, 1.0f };
    loc = glGetUniformLocation(program, "lightPos");
    glUniform4fv(loc, 1, lightPos);

    GLfloat lightDiff[] = { 120.0f, 120.0f, 120.0f, 1.0f };
    loc = glGetUniformLocation(program, "lightDiff");
    glUniform4fv(loc, 1, lightDiff);
}

void SkeletalModel::loadSkeleton(const char *filename)
{
    // Load the skeleton from file here.
    ifstream skeleton(filename, ios::in);
    map<int, Joint *> jointsByIndex;
    map<int, vector<Joint *>> adjacencyList;
    int jointIndex = 0;
    float x, y, z;
    int parIndex;
    while (skeleton >> x >> y >> z >> parIndex)
    {
        Joint *joint = new Joint;
        jointsByIndex[jointIndex] = joint;
        adjacencyList[parIndex].push_back(joint);
        jointIndex += 1;
        Vector4f T(x, y, z, 1.0f);
        /*
        See
        https://en.wikipedia.org/wiki/Translation_(geometry)#Matrix_representation
        for an explanation about translation matrices.
        */
        joint->transform = Matrix4f::translation(x, y, z);
    }
    m_rootJoint = adjacencyList[-1][0];
    assert(m_rootJoint != NULL);
    for (int j = 0; j < jointIndex; ++j)
    {
        // Set the children of each joint.
        jointsByIndex[j]->children = adjacencyList[j];
        // Update the list of joints.
        m_joints.push_back(jointsByIndex[j]);
    }
    assert(m_joints.size() == jointIndex);
}

void SkeletalModel::traverseAndDrawJoint(const Camera &camera, const Joint *joint)
{
    m_matrixStack.push(joint->transform);
    camera.SetUniforms(program, m_matrixStack.top());
    drawSphere(0.025f, 12, 12);
    for (const Joint *j : joint->children)
    {
        traverseAndDrawJoint(camera, j);
    }
    m_matrixStack.pop();
}

void SkeletalModel::drawJoints(const Camera &camera)
{
    // Draw a sphere at each joint. You will need to add a recursive
    // helper function to traverse the joint hierarchy.
    //
    // We recommend using drawSphere( 0.025f, 12, 12 )
    // to draw a sphere of reasonable size.
    //
    // You should use your MatrixStack class. A function
    // should push it's changes onto the stack, and
    // use stack.pop() to revert the stack to the original
    // state.

    // Start at the root and draw everything else.
    traverseAndDrawJoint(camera, m_rootJoint);
}

void SkeletalModel::drawSkeleton(const Camera &camera)
{
    // Draw cylinders between the joints. You will need to add a recursive
    // helper function to traverse the joint hierarchy.
    //
    // We recommend using drawCylinder(6, 0.02f, <height>);
    // to draw a cylinder of reasonable diameter.

    // you can use the stack with push/pop like this
    // m_matrixStack.push(Matrix4f::translation(+0.6f, +0.5f, -0.5f))
    // camera.SetUniforms(program, m_matrixStack.top());
    // drawCylinder(6, 0.02f, 0.2f);
    // callChildFunction();
    // m_matrixStack.pop();
}

void SkeletalModel::setJointTransform(int jointIndex, float rX, float rY, float rZ)
{
    // Set the rotation part of the joint's transformation matrix based on the passed in Euler angles.
}

void SkeletalModel::computeBindWorldToJointTransforms()
{
    // 2.3.1. Implement this method to compute a per-joint transform from
    // world-space to joint space in the BIND POSE.
    //
    // Note that this needs to be computed only once since there is only
    // a single bind pose.
    //
    // This method should update each joint's bindWorldToJointTransform.
    // You will need to add a recursive helper function to traverse the joint hierarchy.

}

void SkeletalModel::updateCurrentJointToWorldTransforms()
{
    // 2.3.2. Implement this method to compute a per-joint transform from
    // joint space to world space in the CURRENT POSE.
    //
    // The current pose is defined by the rotations you've applied to the
    // joints and hence needs to be *updated* every time the joint angles change.
    //
    // This method should update each joint's currentJointToWorldTransform.
    // You will need to add a recursive helper function to traverse the joint hierarchy.

}

void SkeletalModel::updateMesh()
{
    // 2.3.2. This is the core of SSD.
    // Implement this method to update the vertices of the mesh
    // given the current state of the skeleton.
    // You will need both the bind pose world --> joint transforms.
    // and the current joint --> world transforms.
}

