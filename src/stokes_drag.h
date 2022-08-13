#pragma once

#include "force_generator.h"
#include "particle_system.h"

// Calculates force as the result of the drag defined by Stokes' Law.
// Stokes' law assumes the object is a sphere.
//
// Stokes' law makes use of these assumptions:
// - Laminar flow
// - Spherical particles
// - Smooth surfaces
// - No interference between particles
class StokesDrag : public ForceGenerator {
public:
    StokesDrag(int index, float viscosity, float radius);
    virtual void apply(ParticleState *states, ParticleInfo *infos, size_t count);

    // Particle to act on. Index of -1 means; act on all particles
    int index_;

    // Dynamic viscosity
    float mu_;
    // Radius of spherical particle
    float R_;

private:
    void applySingleParticle(ParticleState state, ParticleInfo info);
};
