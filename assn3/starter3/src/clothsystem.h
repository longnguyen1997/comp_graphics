#ifndef CLOTHSYSTEM_H
#define CLOTHSYSTEM_H

#include <vector>

#include "particlesystem.h"
#include "pendulumsystem.h"

class ClothSystem : public PendulumSystem {
    ///ADD MORE FUNCTION AND FIELDS HERE
public:
    ClothSystem();
    int indexOf(int i, int j);
    std::vector<int> directNeighbors(int i, int j);
    std::vector<int> diagonalNeighbors(int i, int j);
    std::vector<int> flexNeighbors(int i, int j);
    std::vector<Spring> springsStructural;
    std::vector<Spring> springsShear;
    std::vector<Spring> springsFlexion;
    int numParticles;
    // evalF is called by the integrator at least once per time step
    std::vector<Vector3f> evalF(std::vector<Vector3f> state) override;

    // draw is called once per frame
    void draw(GLProgram &ctx);

    // inherits
    // std::vector<Vector3f> m_vVecState;
};


#endif
