#pragma once

#include "force_generator.h"
#include "particle.h"

#include <stdlib.h>
#include <vector>

class ParticleSystem {
public:
    size_t vectorSize();
    // TODO: Switch to Eigen::vectors as parameter
    //void statesUpdate(std::vector<Eigen::Vector3f> &updated);
    void statesDerive(std::vector<Eigen::Vector3f> &dst);
    void statesAdd(std::vector<Eigen::Vector3f> &vals);
    uint addParticle(ParticleState initial_state, ParticleInfo initial_info);
    void addForce(ForceGenerator* force);

    ParticleState const& getState(uint index);
    ParticleInfo const& getInfo(uint index);
    uint count();

    float t;  // Simulation clock
private:
    void clearForceAccumulators();
    void updateForceAccumulators();

    // TODO: See if it makes sense to switch to matrices instead of std::vectors
    // with Eigen::vectors inside
    std::vector<ParticleState> particles_state_;
    std::vector<ParticleInfo> particles_info_;
    std::vector<ForceGenerator*> forces_;
};

