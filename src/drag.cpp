#include "drag.h"

#include <assert.h>

Drag::Drag(int index, float fluid_density, float drag_coeff, float cross_area) {
    index_ = index;
    rho_ = fluid_density;
    Cd_ = drag_coeff;
    A_ = cross_area;
}

void Drag::applySingleParticle(ParticleState &state, ParticleInfo &info) {
    // TODO: Add proper vector operations
    for (uint i = 0; i < 3; i++) {
        int sign = state.v[i] < 0 ? 1 : -1;
        info.f[i] += sign * 0.5 * rho_ * Cd_ * A_ * state.v[i] * state.v[i];
    }
}

void Drag::apply(ParticleState *states, ParticleInfo *infos, size_t count) {
    if (index_ == -1) {
        for (size_t i = 0; i < count; i++) {
            applySingleParticle(states[i], infos[i]);
        }
    } else {
        assert(count > (size_t)index_);
        applySingleParticle(states[index_], infos[index_]);
    }
}
