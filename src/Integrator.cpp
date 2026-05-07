#include "../include/Integrator.hpp"


void EulerIntegrator::step(ParticleSystem& system, double dt) {
    std::vector<Particle>& particles = system.getParticles();

    for (auto& p : particles) {
        Vec3 acceleration = p.force / p.mass;
        p.position += p.velocity * dt;
        p.velocity += acceleration * dt;
    }

    system.applyBoundaries();
}


void VelocityVerletIntegrator::step(ParticleSystem& system, double dt) {
    std::vector<Particle>& particles = system.getParticles();
    std::vector<Vec3> oldAccelerations;
    oldAccelerations.reserve(particles.size());

    for (auto& p : particles) {
        oldAccelerations.push_back(p.force / p.mass);
    }

    for (size_t i = 0; i < particles.size(); i++) {
        particles[i].position += particles[i].velocity * dt + oldAccelerations[i] * (0.5 * dt * dt);
    }

    system.applyBoundaries();
    system.computeForces();

    for (size_t i = 0; i < particles.size(); i++) {
        Vec3 newAcceleration = particles[i].force / particles[i].mass;
        particles[i].velocity += (oldAccelerations[i] + newAcceleration) * (0.5 * dt);
    }
}


void LeapfrogIntegrator::step(ParticleSystem& system, double dt) {
    std::vector<Particle>& particles = system.getParticles();

    for (auto& p : particles) {
        Vec3 acceleration = p.force / p.mass;
        p.velocity += acceleration * (0.5 * dt);
    }

    for (auto& p : particles) {
        p.position += p.velocity * dt;
    }

    system.applyBoundaries();
    system.computeForces();

    for (auto& p : particles) {
        Vec3 acceleration = p.force / p.mass;
        p.velocity += acceleration * (0.5 * dt);
    }
}