#include "spring.h"

Spring::Spring(
        uint index0,
        uint index1,
        float rest_length,
        float spring_constant,
        float damping_constant) {
    index0_ = index0;
    index1_ = index1;
    r_ = rest_length;
    ks_ = spring_constant;
    kd_ = damping_constant;
}

void Spring::apply(const ParticleState *states, ParticleInfo *infos, size_t count) {
    assert(index0_ < count && index1_ < count);
    auto &state0 = states[index0_];
    auto &state1 = states[index1_];
    auto &info0 = infos[index0_];
    auto &info1 = infos[index1_];

    auto dx = state0.x - state1.x;
    auto dv = state0.v - state1.v;

    auto dx_len = dx.norm();
    auto spring_comp = ks_ * (dx_len - r_);
    auto damp_comp = kd_ * dv.dot(dx)/dx_len;

    auto f0 = -( spring_comp + damp_comp ) * dx/dx_len;
    auto f1 = -f0;

    info0.f += f0;
    info1.f += f1;
}
