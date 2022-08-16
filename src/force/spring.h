#pragma once

#include "force_generator.h"

class Spring : public ForceGenerator {
public:
    Spring(uint index0, uint index1, float rest_length, float spring_constant,
            float damping_constant);
    void apply(const ParticleState *states, ParticleInfo *infos, size_t count) override;

    // System indices to act on
    uint index0_;
    uint index1_;

    // Rest length
    float r_;
    // Spring constant
    float ks_;
    // Damping constant
    float kd_;
};
