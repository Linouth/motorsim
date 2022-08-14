#include "gravity.h"

Gravity::Gravity() {
    g_ = 9.81;
};

void Gravity::apply(ParticleState *states, ParticleInfo *infos, size_t count) {
    for (size_t i = 0; i < count; i++) {
        auto &info = infos[i];
        info.f[2] -= info.m * g_;
    }
}

