#include "stokes_drag.h"

#include <assert.h>
#include <math.h>

StokesDrag::StokesDrag(int index, float viscosity, float radius) {
    index_ = index;
    mu_ = viscosity;
    R_ = radius;
}

void StokesDrag::applySingleParticle(ParticleState &state, ParticleInfo &info) {
    for (uint i = 0; i < 3; i++) {
        info.f[i] -= 6 * M_PI * mu_ * R_ * state.v[i];
    }
}

void StokesDrag::apply(ParticleState *states, ParticleInfo *infos, size_t count) {
    if (index_ == -1) {
        for (size_t i = 0; i < count; i++) {
            applySingleParticle(states[i], infos[i]);
        }
    } else {
        assert(count > (size_t)index_);
        applySingleParticle(states[index_], infos[index_]);
    }
}
