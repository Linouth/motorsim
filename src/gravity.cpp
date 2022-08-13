#include "particle_system.h"
#include "gravity.h"


Gravity::Gravity() {
    g_ = 9.81;
};

void Gravity::apply(ParticleState *states, ParticleInfo *infos, size_t count) {
    for (size_t i = 0; i < count; i++) {
        infos->f[2] -= infos->m * g_;
    }
}

