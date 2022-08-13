#pragma once

#include "stdlib.h"

#include "particle.h"

class ForceGenerator {
public:
    virtual void apply(ParticleState *states, ParticleInfo *infos, size_t count) = 0;
};

