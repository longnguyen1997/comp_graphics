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
    float h = stepSize;
    vector<Vector3f> k1, k2, k3, k4;
    vector<Vector3f> x = particleSystem->getState();

    k1 = particleSystem->evalF(x);
    vector<Vector3f> hDiv2k1;
    for (unsigned int i = 0; i < x.size(); ++i) {
        hDiv2k1.push_back(
            x[i] + (h / 2 * k1[i])
        );
    }

    k2 = particleSystem->evalF(hDiv2k1);
    vector<Vector3f> hDiv2k2;
    for (unsigned int i = 0; i < x.size(); ++i) {
        hDiv2k2.push_back(
            x[i] + (h / 2 * k2[i])
        );
    }

    k3 = particleSystem->evalF(hDiv2k2);
    vector<Vector3f> hk3;
    for (unsigned int i = 0; i < x.size(); ++i) {
        hk3.push_back(
            x[i] + (h * k3[i])
        );
    }

    k4 = particleSystem->evalF(hk3);

    vector<Vector3f> XPrime;
    for (int i = 0; i < (int)x.size(); ++i) {
        XPrime.push_back(
            x[i] + (h / 6) * (k1[i] + 2 * (k2[i] + k3[i]) + k4[i])
        );
    }
    particleSystem->setState(XPrime);
}

