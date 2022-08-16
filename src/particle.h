#pragma once

#include <Eigen/Dense>

typedef Eigen::Vector3f Vec;

struct ParticleState {
    //Position
    Vec x;
    // Velocity
    Vec v;
};

struct ParticleInfo {
    // Mass of the particle
    float m;
    // Force accumulator
    Vec f;
};
