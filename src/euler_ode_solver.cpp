#include "euler_ode_solver.h"

EulerOdeSolver::EulerOdeSolver() {
    derivitives = std::vector<float>(512);
}

void EulerOdeSolver::step(ParticleSystem &p, float dt) {
    p.statesDerive(derivitives);

    // Scale derivitives with time step
    for (auto &d : derivitives) {
        d *= dt;
    }
    p.statesAdd(derivitives);

    p.t += dt;
}
