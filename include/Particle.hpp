#pragma once

#include <vector>
#include <random>
#include "MathCore.hpp"

extern std::mt19937 gen;


struct Particle {
    Vec3 position;
    Vec3 velocity;
    Vec3 force;
    float mass;

    Particle();
};

Particle createParticle(Vec3 position, Vec3 velocity, float mass);

Vec3 randomPosition(float boxSize);
Vec3 randomVelocity(float sigma);

std::vector<Particle> createParticles(int count, float mass, float boxSize, float v_max);

class ParticleSystem {
private:
    std::vector<Particle> particles;
    float boxSize;
    float cutoff;
    float epsilon;
    float sigma;
    float sigma6;
    float sigma12;
    float mass;
    float U_cut;

public:
    ParticleSystem(int count, float mass, float boxSize, float v_max, float epsilon, float sigma, int integratorMode);

    void resetForces();
    void computeForces();
    void integrate(float dt);
    void applyBoundaries();
    void initParticles(int count, float mass, float boxSize, float v_max, float epsilon, float sigma, int integratorMode);

    Vec3 computeAverageVelocity() const;
    Vec3 computeSeparation(int i, int j) const;

    float kineticEnergy() const;
    float potentialEnergy() const;
    float temperature() const;
    std::vector<Particle>& getParticles();
    const std::vector<Particle>& getParticles() const;
    int size() const;
    int integratorMode;
    float getBoxSize() const;
};