#include "clothsystem.h"
#include "camera.h"
#include "vertexrecorder.h"
#include <iostream>
using namespace std;
// your system should at least contain 8x8 particles.
const int W = 8;
const int H = 8;
const float rStruct = 0.5f;
const float rShear = 0.7f;
const float rFlex = 1.0f;

vector<int> ClothSystem::directNeighbors(int i, int j) {
    vector<vector<int>> neighborCoords{{i - 1, j},
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
    vector<vector<int>> neighborCoords{{i - 1, j - 1},
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
    vector<vector<int>> neighborCoords{{i, j - 2},
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

    // 0 initial velocity.
    velocities = std::vector<Vector3f>(W * H, Vector3f(0, 0, 0));
    for (int w = 0; w < W; ++w) {
        for (int h = 0; h < H; ++h) {
            // Positions in a grid.
            positions.push_back(Vector3f(2.0f / W * w, 2.0f / H * h, 0));
        }
    }

    // Structural springs.
    springsStructural = vector<Spring>(W * H, Spring(rStruct));
    for (int i = 0; i < W; ++i) {
        for (int j = 0; j < H; ++j) {
            for (int neighbor : directNeighbors(i, j)) {
                springsStructural[indexOf(i, j)].addConnection(neighbor);
            }
        }
    }

    // Shear springs.
    springsShear = vector<Spring>(W * H, Spring(rShear));
    for (int i = 0; i < W; ++i) {
        for (int j = 0; j < H; ++j) {
            for (int neighbor : diagonalNeighbors(i, j)) {
                springsShear[indexOf(i, j)].addConnection(neighbor);
            }
        }
    }

    // Flexion springs.
    springsFlexion = vector<Spring>(W * H, Spring(rFlex));
    for (int i = 0; i < W; ++i) {
        for (int j = 0; j < H; ++j) {
            for (int neighbor : flexNeighbors(i, j)) {
                springsFlexion[indexOf(i, j)].addConnection(neighbor);
            }
        }
    }
}

std::vector<Vector3f> ClothSystem::evalF(std::vector<Vector3f> state) {
    std::vector<Vector3f> f(state.size());
    // TODO 5. implement evalF
    // - gravity
    // - viscous drag
    // - structural springs
    // - shear springs
    // - flexion springs

    return f;
}


void ClothSystem::draw(GLProgram &gl) {
    //TODO 5: render the system
    //         - ie draw the particles as little spheres
    //         - or draw the springs as little lines or cylinders
    //         - or draw wireframe mesh

    const Vector3f CLOTH_COLOR(0.9f, 0.9f, 0.9f);
    gl.updateMaterial(CLOTH_COLOR);

    // EXAMPLE for how to render cloth particles.
    //  - you should replace this code.
    float w = 0.2f;
    Vector3f O(-1, 1, -1);
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

