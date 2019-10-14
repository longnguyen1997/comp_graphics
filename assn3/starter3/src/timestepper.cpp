#include "timestepper.h"

#include <cstdio>
using namespace std;

void ForwardEuler::takeStep(ParticleSystem *particleSystem, float stepSize) {
    //TODO: See handout 3.1
    vector<Vector3f> X = particleSystem->getState();
    vector<Vector3f> F = particleSystem->evalF(X);
    vector<Vector3f> XPrime;
    for (int i = 0; i < (int)X.size(); ++i) {
        XPrime.push_back(X[i] + stepSize * F[i]);
    }
    particleSystem->setState(XPrime);
}

void Trapezoidal::takeStep(ParticleSystem *particleSystem, float stepSize) {
    //TODO: See handout 3.1
    vector<Vector3f> X = particleSystem->getState();
    vector<Vector3f> f0 = particleSystem->evalF(X);
    vector<Vector3f> XPlusHf0;
    for (int i = 0; i < (int)X.size(); ++i) {
        XPlusHf0.push_back(X[i] + stepSize * f0[i]);
    }
    vector<Vector3f> f1 = particleSystem->evalF(XPlusHf0);
    vector<Vector3f> XPrime;
    for (int i = 0; i < (int)X.size(); ++i) {
        XPrime.push_back(X[i] + (
                             (stepSize / 2) * (f0[i] + f1[i])
                         )
                        );
    }
    particleSystem->setState(XPrime);
}


void RK4::takeStep(ParticleSystem *particleSystem, float stepSize) {
}

