#pragma once

#include <vector>
#include <Eigen/Dense>

#include "particle_system.h"

class EulerOdeSolver {
public:
    EulerOdeSolver();

    void step(ParticleSystem &p, float dt);

private:
    std::vector<Eigen::Vector3f> derivitives;
};
