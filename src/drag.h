#pragma once

#include "force_generator.h"
#include "particle.h"

// Implementation for the square drag equation
class Drag : public ForceGenerator {
public:
    Drag(int index, float fluid_density, float drag_coeff, float cross_area);
    void apply(ParticleState *states, ParticleInfo *infos, size_t count);

    // Particle to act on. Index of -1 means; act on all particles
    int index_;

    // Density of fluid
    float rho_;
    // Drag coefficient
    float Cd_;
    // Cross-sectional area
    float A_;

private:
    // TODO: Check if references are quicker than copying the params (learning)
    void applySingleParticle(ParticleState &state, ParticleInfo &info);
};
