#include "pendulumsystem.h"

#include <cassert>
#include "camera.h"
#include "vertexrecorder.h"

using namespace std;

// TODO adjust to number of particles.
const int NUM_PARTICLES = 5;
// Gravity vector (y direction)
const Vector3f g(0, -9.8, 0);
// Mass of an object
const float m = 0.05;
// Viscous drag constant
const float kDrag = 0.05;
// Spring constant
const float kSpring = 1.5;
// Resting spring length
const float restingLen = 0.02;

Spring::Spring() {
    // Any extra logic here.
}

Spring::Spring(float r) {
    restLen = r;
}

void Spring::addConnection(int i) {
    connections.push_back(i);
}

vector<int> Spring::getConnections() {
    return connections;
}

PendulumSystem::PendulumSystem() {
    // To add a bit of randomness, use e.g.
    // float f = rand_uniform(-0.5f, 0.5f);
    // in your initial conditions.

    // Initial position and velocity.
    vector<Vector3f> initialConds;

    // TODO 4.2 Add particles for simple pendulum
    // TODO 4.3 Extend to multiple particles
    for (int i = 0; i < NUM_PARTICLES; ++i) {
        float f = rand_uniform(-0.5f, 0.5f);
        Vector3f X, V;
        X = Vector3f(0.5 + f, 1.0 + f, f);
        V = Vector3f(0.0, 0.0, 0.0);
        initialConds.push_back(X);
        initialConds.push_back(V);
        // Initial point in the spring, fixed.
        if (i == 0) {
            springs.push_back(Spring());
        } else { // For other particles.
            Spring spring = Spring();
            // Connect both neighbors.
            spring.addConnection(i - 1);
            springs[i - 1].addConnection(i);
            springs.push_back(spring);
        }
    }
    setState(initialConds);
}

vector<Vector3f> getPositions(vector<Vector3f> &state) {
    vector<Vector3f> positions;
    for (unsigned int i = 0; i < state.size(); i += 2) {
        positions.push_back(state[i]);
    }
    return positions;
}

vector<Vector3f> getVelocities(vector<Vector3f> &state) {
    vector<Vector3f> velocities;
    for (unsigned int i = 1; i < state.size(); i += 2) {
        velocities.push_back(state[i]);
    }
    return velocities;
}

std::vector<Vector3f> PendulumSystem::evalF(std::vector<Vector3f> state) {
    vector<Vector3f> positions = getPositions(state);
    vector<Vector3f> velocities = getVelocities(state);
    assert(positions.size() == velocities.size());
    std::vector<Vector3f> f;
    // Fix the initial point.
    f.push_back(Vector3f(0.0, 0.0, 0.0));
    f.push_back(Vector3f(0.0, 0.0, 0.0));
    // TODO 4.1: implement evalF
    for (unsigned int i = 1; i < positions.size(); ++i) {
        //  - gravity
        const Vector3f gravity = m * g;
        //  - viscous drag
        const Vector3f viscousDrag = -kDrag * velocities[i];
        //  - springs
        Vector3f springForces(0, 0, 0);
        const vector<int> connections = springs[i].getConnections();
        for (int c : connections) {
            const Vector3f d = positions[i] - positions[c];
            const Vector3f F = -kSpring * (d.abs() - restingLen) * d.normalized();
            springForces += F;
        }
        // net force
        const Vector3f netForce = gravity + viscousDrag + springForces;
        // X'' = F(X, X')
        const Vector3f acceleration = netForce / m;
        // dX/dt = f(X, t) = <v, F(x,v)>
        f.push_back(velocities[i]);
        f.push_back(acceleration);
    }
    return f;
}

// render the system (ie draw the particles)
void PendulumSystem::draw(GLProgram &gl) {
    const Vector3f PENDULUM_COLOR(0.73f, 0.0f, 0.83f);
    gl.updateMaterial(PENDULUM_COLOR);

    // TODO 4.2, 4.3
    vector<Vector3f> state = getState();
    for (int i = 0; i < (int)state.size(); i += 2) {
        Vector3f pos = state[i]; // Particle n's position.
        gl.updateModelMatrix(Matrix4f::translation(pos));
        drawSphere(0.075f, 10, 10);
    }
}
