#include <assert.h>
#include <cstring>

#include "particle_system.h"


size_t ParticleSystem::vectorSize() {
    // TODO: This can be done at compile time automatically
    // (sizeof(ParticleState)/sizeof(float))
    return 6 * particles_state_.size();
}

//void ParticleSystem::statesUpdate(std::vector<Eigen::Vector3f> &updated) {
//    assert(particles_state_.size() == updated.size());
//    //std::copy(updated.begin(), updated.end(), particles_state_.begin());
//    // TODO: What is the 'proper' c++ way to do this? 
//    std::memcpy(particles_state_.data(), updated.data(), updated.size());
//}

void ParticleSystem::statesDerive(std::vector<Eigen::Vector3f> &dst) {
    dst.clear();

    // Reset all forces
    clearForceAccumulators();
    // Calculate all forces as result of the Force generators, and update the
    // accumulators.
    updateForceAccumulators();

    for (size_t i = 0; i < particles_state_.size(); i++) {
        auto state = particles_state_.at(i);
        auto info = particles_info_.at(i);

        // x_dot = v
        dst.push_back(state.v);
        // v_dot = f/m
        dst.push_back(info.f / info.m);
    }
}

void ParticleSystem::statesAdd(std::vector<Eigen::Vector3f> &vals) {
    size_t ind = 0;

    Eigen::Vector3f* s = reinterpret_cast<Eigen::Vector3f*>(particles_state_.data());
    const uint state_size = sizeof(ParticleState) / sizeof(Eigen::Vector3f);
    for (size_t i = 0; i < particles_state_.size() * state_size; i++) {
        s[i] += vals.at(ind++);
    }
}

uint ParticleSystem::addParticle(ParticleState initial_state, ParticleInfo initial_info) {
    particles_state_.push_back(initial_state);
    particles_info_.push_back(initial_info);
    return particles_state_.size() - 1;
}

void ParticleSystem::addForce(ForceGenerator* force) {
    forces_.push_back(force);
}

ParticleState const& ParticleSystem::getState(uint index) {
    return particles_state_.at(index);
}

ParticleInfo const& ParticleSystem::getInfo(uint index) {
    return particles_info_.at(index);
}

uint ParticleSystem::count() {
    return particles_state_.size();
}

void ParticleSystem::clearForceAccumulators() {
    // TODO: Again, more general method, so nth dimensional vectors can be used
    for (auto &info : particles_info_) {
        info.f[0] = info.f[1] = info.f[2] = 0;
    }
}

void ParticleSystem::updateForceAccumulators() {
    for (auto &force : forces_) {
        force->apply(particles_state_.data(), particles_info_.data(),
                particles_info_.size());
    }
}
