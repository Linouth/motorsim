#include "stokes_drag.h"

#include <assert.h>
#include <math.h>

StokesDrag::StokesDrag(int index, float viscosity, float radius) {
    index_ = index;
    mu_ = viscosity;
    R_ = radius;
}

void StokesDrag::applySingleParticle(const ParticleState &state, ParticleInfo &info) {
    info.f -= 6 * M_PI * mu_ * R_ * state.v;
}

void StokesDrag::apply(const ParticleState *states, ParticleInfo *infos, size_t count) {
    if (index_ == -1) {
        for (size_t i = 0; i < count; i++) {
            applySingleParticle(states[i], infos[i]);
        }
    } else {
        assert(count > (size_t)index_);
        applySingleParticle(states[index_], infos[index_]);
    }
}
