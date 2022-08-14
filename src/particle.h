#pragma once

#include <Eigen/Dense>

struct ParticleState {
    //Position
    Eigen::Vector3f x;
    // Velocity
    Eigen::Vector3f v;
};

struct ParticleInfo {
    // Mass of the particle
    float m;
    // Force accumulator
    Eigen::Vector3f f;
};
