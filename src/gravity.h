#pragma once

#include "force_generator.h"
#include "particle_system.h"

class Gravity : public ForceGenerator {
public:
    Gravity();
    virtual void apply(ParticleState *states, ParticleInfo *infos, size_t count);

    // Gravitational constant
    float g_;
};
