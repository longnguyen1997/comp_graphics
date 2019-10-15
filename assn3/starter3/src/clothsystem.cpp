#include "clothsystem.h"
#include "camera.h"
#include "vertexrecorder.h"
#include <iostream>
using namespace std;

// your system should at least contain 8x8 particles.
const int W = 10;
const int H = 10;
const float rStruct = 0.5f;
const float rShear = 0.7f;
const float rFlex = 1.0f;

// Gravity vector (y direction)
const Vector3f g(0, -9.8, 0);
// Mass of a cloth particle
const float m = 0.01;
// Viscous drag constant
const float kDrag = 0.1;
// Spring constant
const float kSpring = 10;
const float clothWidth = 2.5f;

vector<int> ClothSystem::directNeighbors(int i, int j) {
    vector<vector<int>> neighborCoords{
        {i - 1, j},
        {i, j - 1},
        {i, j + 1},
        {i + 1, j}
    };
    vector<int> neighbors;
    for (vector<int> neighbor : neighborCoords) {
        const int x = neighbor[0];
        const int y = neighbor[1];
        if (x >= 0 && y >= 0 && x < W && y < H) {
            neighbors.push_back(indexOf(x, y));
        }
    }
    return neighbors;
}

vector<int> ClothSystem::diagonalNeighbors(int i, int j) {
    vector<vector<int>> neighborCoords{
        {i - 1, j - 1},
        {i + 1, j - 1},
        {i - 1, j + 1},
        {i + 1, j + 1}
    };
    vector<int> neighbors;
    for (vector<int> neighbor : neighborCoords) {
        const int x = neighbor[0];
        const int y = neighbor[1];
        if (x >= 0 && y >= 0 && x < W && y < H) {
            neighbors.push_back(indexOf(x, y));
        }
    }
    return neighbors;
}

vector<int> ClothSystem::flexNeighbors(int i, int j) {
    vector<vector<int>> neighborCoords{
        {i, j - 2},
        {i, j + 2},
        {i - 2, j},
        {i + 2, j}
    };
    vector<int> neighbors;
    for (vector<int> neighbor : neighborCoords) {
        const int x = neighbor[0];
        const int y = neighbor[1];
        if (x >= 0 && y >= 0 && x < W && y < H) {
            neighbors.push_back(indexOf(x, y));
        }
    }
    return neighbors;
}

int ClothSystem::indexOf(int i, int j) {
    return i * W + j;
}

ClothSystem::ClothSystem() {
    // TODO 5. Initialize m_vVecState with cloth particles.
    // You can again use rand_uniform(lo, hi) to make things a bit more interesting

    numParticles = W * H;

    vector<Vector3f> state;

    for (int h = 0; h < H; ++h) {
        for (int w = 0; w < W; ++w) {
            // Positions in a grid.
            state.push_back(Vector3f(
                                clothWidth / W * w,
                                clothWidth / H * -h,
                                rand_uniform(-0.1f, 0.1f)
                            ));
            // 0 initial velocity.
            state.push_back(Vector3f(0));
        }
    }

    springsStructural = vector<Spring>(numParticles, Spring());
    springsShear = vector<Spring>(numParticles, Spring());
    springsFlexion = vector<Spring>(numParticles, Spring());
    for (int i = 0; i < W; ++i) {
        for (int j = 0; j < H; ++j) {
            // Structural springs.
            for (int neighbor : directNeighbors(i, j)) {
                springsStructural[indexOf(i, j)].addConnection(neighbor);
            }
            // Shear springs.
            for (int neighbor : diagonalNeighbors(i, j)) {
                springsShear[indexOf(i, j)].addConnection(neighbor);
            }
            // Flexion springs.
            for (int neighbor : flexNeighbors(i, j)) {
                springsFlexion[indexOf(i, j)].addConnection(neighbor);
            }
        }
    }

    setState(state);
}

vector<Vector3f> extractPos(vector<Vector3f> &state) {
    vector<Vector3f> positions;
    for (unsigned int i = 0; i < state.size(); i += 2) {
        positions.push_back(state[i]);
    }
    return positions;
}

vector<Vector3f> extractVel(vector<Vector3f> &state) {
    vector<Vector3f> velocities;
    for (unsigned int i = 1; i < state.size(); i += 2) {
        velocities.push_back(state[i]);
    }
    return velocities;
}

std::vector<Vector3f> ClothSystem::evalF(std::vector<Vector3f> state) {
    std::vector<Vector3f> f;
    vector<Vector3f> positions = extractPos(state);
    vector<Vector3f> velocities = extractVel(state);
    // TODO 5. implement evalF
    for (unsigned int i = 0; i < positions.size(); ++i) {
        //  - gravity
        const Vector3f gravity = m * g;
        //  - viscous drag
        const Vector3f viscousDrag = -kDrag * velocities[i];
        //  - structural springs
        Vector3f springStructForce(0, 0, 0);
        for (int c : springsStructural[i].getConnections()) {
            const Vector3f d = positions[i] - positions[c];
            const Vector3f F = -kSpring * (d.abs() - rStruct) * d.normalized();
            springStructForce += F;
        }
        //  - shear springs
        Vector3f springShearForce(0, 0, 0);
        for (int c : springsShear[i].getConnections()) {
            const Vector3f d = positions[i] - positions[c];
            const Vector3f F = -kSpring * (d.abs() - rShear) * d.normalized();
            springShearForce += F;
        }
        //  - flexion springs
        Vector3f springFlexForce(0, 0, 0);
        for (int c : springsFlexion[i].getConnections()) {
            const Vector3f d = positions[i] - positions[c];
            const Vector3f F = -kSpring * (d.abs() - rFlex) * d.normalized();
            springFlexForce += F;
        }
        // all spring forces together
        Vector3f springForces = springStructForce + springShearForce + springFlexForce;
        // net force
        const Vector3f netForce = gravity + viscousDrag + springForces;
        // X'' = F(X, X')
        const Vector3f acceleration = netForce / m;
        // dX/dt = f(X, t) = <v, F(x,v)>
        // Top corners should stay still for the cloth.
        f.push_back((i == 0 || i == W - 1) ? Vector3f(0) : velocities[i]);
        f.push_back((i == 0 || i == W - 1) ? Vector3f(0) : acceleration);
    }
    return f;
}

void ClothSystem::draw(GLProgram &gl) {
    //TODO 5: render the system
    //         - ie draw the particles as little spheres
    //         - or draw the springs as little lines or cylinders
    //         - or draw wireframe mesh

    const Vector3f CLOTH_COLOR(0.9f, 0.9f, 0.9f);
    gl.updateMaterial(CLOTH_COLOR);

    vector<Vector3f> positions = extractPos(m_vVecState);

    // EXAMPLE for how to render cloth particles.
    //  - you should replace this code.
    float w = 0.2f;
    // Offset in the scene world
    Vector3f O(-2.5, 2.5, -1.5);
    for (Vector3f X : positions) {
        gl.updateModelMatrix(Matrix4f::translation(O + X));
        drawSphere(0.04f, 8, 8);
    }

    /*
    gl.updateModelMatrix(Matrix4f::translation(O));
    drawSphere(0.04f, 8, 8);
    gl.updateModelMatrix(Matrix4f::translation(O + Vector3f(w, 0, 0)));
    drawSphere(0.04f, 8, 8);
    gl.updateModelMatrix(Matrix4f::translation(O + Vector3f(w, -w, 0)));
    drawSphere(0.04f, 8, 8);
    gl.updateModelMatrix(Matrix4f::translation(O + Vector3f(0, -w, 0)));
    drawSphere(0.04f, 8, 8);
    */

    // EXAMPLE: This shows you how to render lines to debug the spring system.
    //
    //          You should replace this code.
    //
    //          Since lines don't have a clearly defined normal, we can't use
    //          a regular lighting model.
    //          GLprogram has a "color only" mode, where illumination
    //          is disabled, and you specify color directly as vertex attribute.
    //          Note: enableLighting/disableLighting invalidates uniforms,
    //          so you'll have to update the transformation/material parameters
    //          after a mode change.
    /*
    gl.disableLighting();
    gl.updateModelMatrix(Matrix4f::identity()); // update uniforms after mode change
    VertexRecorder rec;
    rec.record(O, CLOTH_COLOR);
    rec.record(O + Vector3f(w, 0, 0), CLOTH_COLOR);
    rec.record(O, CLOTH_COLOR);
    rec.record(O + Vector3f(0, -w, 0), CLOTH_COLOR);

    rec.record(O + Vector3f(w, 0, 0), CLOTH_COLOR);
    rec.record(O + Vector3f(w, -w, 0), CLOTH_COLOR);

    rec.record(O + Vector3f(0, -w, 0), CLOTH_COLOR);
    rec.record(O + Vector3f(w, -w, 0), CLOTH_COLOR);
    glLineWidth(3.0f);
    rec.draw(GL_LINES);

    gl.enableLighting(); // reset to default lighting model
    // EXAMPLE END
    */

    gl.disableLighting();
    gl.updateModelMatrix(Matrix4f::identity());
    VertexRecorder rec;

    for (unsigned int i = 0; i < positions.size(); ++i) {
        Vector3f X = positions[i];
        // Draw ALL springs.
        vector<Spring> springs{springsStructural[i],
                               springsShear[i],
                               springsFlexion[i]
                              };
        for (Spring spring : springs) {
            vector<int> connections = spring.getConnections();
            for (int c : connections) {
                Vector3f connection = positions[c];
                Vector3f diff = connection - X;
                rec.record(O + X, CLOTH_COLOR);
                rec.record(O + X + diff, CLOTH_COLOR);
            }
        }
    }
    glLineWidth(3.0f);
    rec.draw(GL_LINES);
    gl.enableLighting();

}

