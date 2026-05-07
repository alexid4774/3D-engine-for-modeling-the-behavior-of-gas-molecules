#pragma once

#include <vector>
#include <random>
#include "MathCore.hpp"

extern std::mt19937 gen;


struct Particle {
    Vec3 position;
    Vec3 velocity;
    Vec3 force;
    double mass;

    Particle();
};

Particle createParticle(Vec3 position, Vec3 velocity, double mass);

Vec3 randomPosition(double boxSize);
Vec3 randomVelocity(double sigma);

std::vector<Particle> createParticles(int count, double mass, double boxSize, double v_max);

class ParticleSystem {
private:
    std::vector<Particle> particles;
    double boxSize;
    double cutoff;
    double epsilon;
    double sigma;
    double sigma6;
    double sigma12;
    double mass;
    double U_cut;

public:
    ParticleSystem(int count, double mass, double boxSize, double v_max, double epsilon, double sigma);

    void resetForces();
    void computeForces();
    void integrate(double dt);
    void applyBoundaries();
    void initParticles(int count, double mass, double boxSize, double v_max, double epsilon, double sigma);

    Vec3 computeAverageVelocity() const;
    Vec3 computeSeparation(int i, int j) const;

    double kineticEnergy() const;
    double potentialEnergy() const;
    double temperature() const;
    std::vector<Particle>& getParticles();
    const std::vector<Particle>& getParticles() const;
    int size() const;
};