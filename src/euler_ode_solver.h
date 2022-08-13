#pragma once

#include <vector>

#include "particle_system.h"

class EulerOdeSolver {
public:
    EulerOdeSolver();

    void step(ParticleSystem &p, float dt);

private:
    std::vector<float> derivitives;
};
