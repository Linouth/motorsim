#include "drag.h"

#include <assert.h>

Drag::Drag(int index, float fluid_density, float drag_coeff, float cross_area) {
    index_ = index;
    rho_ = fluid_density;
    Cd_ = drag_coeff;
    A_ = cross_area;
}

void Drag::applySingleParticle(ParticleState &state, ParticleInfo &info) {
    // NOTE: using the coefficient-wise abs, so that directional information is
    // not removed due to the square.
    info.f -= 0.5 * rho_ * Cd_ * A_ * state.v.cwiseAbs().cwiseProduct(state.v);
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
