#ifndef PENDULUMSYSTEM_H
#define PENDULUMSYSTEM_H

#include <vector>

#include "particlesystem.h"

class Spring {
public:
    Spring();
    Spring(float r);
    void addConnection(int i);
    std::vector<int> getConnections();
private:
    std::vector<int> connections;
    float restLen;
};

class PendulumSystem : public ParticleSystem {
public:
    PendulumSystem();

    std::vector<Vector3f> evalF(std::vector<Vector3f> state) override;
    void draw(GLProgram &);

    // inherits
    // std::vector<Vector3f> m_vVecState;
    std::vector<Spring> springs;
};

#endif
