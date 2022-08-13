#pragma once

#include "force_generator.h"
#include "particle.h"

#include <stdlib.h>
#include <vector>

class ParticleSystem {
public:
    size_t vectorSize();
    void statesUpdate(std::vector<float> &updated);
    void statesDerive(std::vector<float> &dst);
    void statesAdd(std::vector<float> &vals);
    uint addParticle(ParticleState initial_state, ParticleInfo initial_info);
    void addForce(ForceGenerator* force);

    ParticleState const& getState(uint index);

    float t;  // Simulation clock
private:
    void clearForceAccumulators();
    void updateForceAccumulators();

    std::vector<ParticleState> particles_state_;
    std::vector<ParticleInfo> particles_info_;
    std::vector<ForceGenerator*> forces_;
};

