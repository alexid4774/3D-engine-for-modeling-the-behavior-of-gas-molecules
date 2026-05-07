#pragma once

#include "Particle.hpp"


class Integrator {
public:
    virtual ~Integrator() = default;

    virtual void step(ParticleSystem& system, double dt) = 0;
};


class EulerIntegrator : public Integrator {
public:
    void step(ParticleSystem& system, double dt) override;
};


class VelocityVerletIntegrator : public Integrator {
public:
    void step(ParticleSystem& system, double dt) override;
};


class LeapfrogIntegrator : public Integrator {
public:
    void step(ParticleSystem& system, double dt) override;
};