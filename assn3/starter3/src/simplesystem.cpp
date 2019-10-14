#include "simplesystem.h"

#include "camera.h"
#include "vertexrecorder.h"
#include <iostream>
using namespace std;

SimpleSystem::SimpleSystem() {
    // TODO 3.2 initialize the simple system
    m_vVecState.push_back(Vector3f(1.0, 0.0, 0.0));
}

std::vector<Vector3f> SimpleSystem::evalF(std::vector<Vector3f> state) {
    std::vector<Vector3f> f;

    // TODO 3.2: implement evalF
    // for a given state, evaluate f(X,t)
    for (int i = 0; i < (int)state.size(); ++i) {
        f.push_back(Vector3f(-state[i].y(), state[i].x(), 0));
    }
    return f;
}

// render the system (ie draw the particles)
void SimpleSystem::draw(GLProgram &gl) {

    // TODO 3.2: draw the particle.
    //           we provide code that draws a static sphere.
    //           you should replace it with your own
    //           drawing code.
    //           In this assignment, you must manage two
    //           kinds of uniforms before you draw
    //            1. Update material uniforms (color)
    //            2. Update transform uniforms
    //           GLProgram is a helper object that has
    //           methods to set the uniform state.
    const Vector3f PARTICLE_COLOR(0.4f, 0.7f, 1.0f);
    gl.updateMaterial(PARTICLE_COLOR);
    vector<Vector3f> state = getState();
    for (int i = 0; i < (int)state.size(); ++i) {
        Vector3f pos = state[i]; //YOUR PARTICLE POSITION
        gl.updateModelMatrix(Matrix4f::translation(pos));
        drawSphere(0.075f, 10, 10);
    }
}
