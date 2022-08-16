#pragma once

#include "force_generator.h"
#include "../particle.h"

class Gravity : public ForceGenerator {
public:
    Gravity();
    void apply(const ParticleState *states, ParticleInfo *infos, size_t count) override;

    // Gravitational constant
    float g_;
};
