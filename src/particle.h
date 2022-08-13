#pragma once

struct ParticleState {
    float x[3];  // Position
    float v[3];  // Velocity
};

struct ParticleInfo {
    // Mass of the particle
    float m;
    // Force accumulator
    float f[3];
};
