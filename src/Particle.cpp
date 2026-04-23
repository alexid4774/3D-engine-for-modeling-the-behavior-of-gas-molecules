#include <iostream>
#include <vector>
#include <random>
#include "MathCore.cpp"


std::mt19937 gen(42);


struct Particle {

    Vec3 position;
    Vec3 velocity;
    Vec3 force;
    double mass;


    Particle() : position(0,0,0), velocity(0,0,0), force(0,0,0), mass(1.0) {}
};


Particle createParticle(Vec3 position, Vec3 velocity, double mass) {

    Particle p;

    p.position = position;
    p.velocity = velocity;
    p.mass = mass;
    p.force = Vec3(0, 0, 0);

    return p;
}


Vec3 randomPosition(double boxSize) {
    std::uniform_real_distribution<double> dist(0, boxSize);

    return Vec3(dist(gen), dist(gen), dist(gen));
}


Vec3 randomVelocity(double sigma) {
    std::normal_distribution<double> dist(0.0, sigma);

    return Vec3(dist(gen), dist(gen), dist(gen));
}


std::vector<Particle> createParticles(int count, double mass, double boxSize, double v_max) {
    std::vector<Particle> particles;
    particles.reserve(count);

    for (int i = 0; i < count; i++) {

        Vec3 position = randomPosition(boxSize);
        Vec3 velocity = randomVelocity(v_max / 3);

        particles.push_back(createParticle(position, velocity, mass));
    }

    return particles;
}


class ParticleSystem {

private:

    std::vector<Particle> particles;
    double boxSize;
    double cutoff;
    double epsilon;
    double sigma;
    double sigma6;
    double sigma12;

public:

    ParticleSystem(int count, double mass, double boxSize, double v_max, double epsilon, double sigma) {

        this->boxSize = boxSize;
        this->particles = createParticles(count, mass, boxSize, v_max);
        this->cutoff = boxSize * 0.99;
        this->epsilon = epsilon;
        this->sigma = sigma;

        double sigma2 = this->sigma * this->sigma;
        this->sigma6 = sigma2 * sigma2 * sigma2;
        this->sigma12 = this->sigma6 * this->sigma6;

    }

    void resetForces() {
        for (auto& p : this->particles)
            p.force = Vec3(0,0,0);
    }

    void computeForces() {

        resetForces();

        double cutoff2 = this->cutoff * this->cutoff;


        for (int i = 0; i < particles.size(); i++) {
            for (int j = i + 1; j < particles.size(); j++) {

                Vec3 r = particles[i].position - particles[j].position;

                // --- Minimum image convention (PBC) ---
                if (r.x >  0.5 * this->boxSize) r.x -= this->boxSize;
                if (r.x < -0.5 * this->boxSize) r.x += this->boxSize;

                if (r.y >  0.5 * this->boxSize) r.y -= this->boxSize;
                if (r.y < -0.5 * this->boxSize) r.y += this->boxSize;

                if (r.z >  0.5 * this->boxSize) r.z -= this->boxSize;
                if (r.z < -0.5 * this->boxSize) r.z += this->boxSize;

                double r2 = r.lengthSq();

                // --- cutoff ---
                if (r2 > cutoff2 || r2 == 0.0) continue;

                double inv_r2 = 1.0 / r2;
                double inv_r6 = inv_r2 * inv_r2 * inv_r2;
                double inv_r12 = inv_r6 * inv_r6;

                // --- Lennard-Jones force scalar ---
                double forceScalar =
                    24.0 * this->epsilon *
                    (2.0 * this->sigma12 * inv_r12 - this->sigma6 * inv_r6) * inv_r2;

                // --- force vector ---
                Vec3 f = r * forceScalar;

                // --- Newton's 3rd law ---
                particles[i].force += f;
                particles[j].force -= f;
            }
        }
    }

    void integrate(double dt);


    void applyBoundaries() {
        for (auto& p : this->particles) {

            if (p.position.x > this->boxSize) p.position.x -= this->boxSize;
            else if (p.position.x < 0) p.position.x += this->boxSize;

            if (p.position.y > this->boxSize) p.position.y -= this->boxSize;
            else if (p.position.y < 0) p.position.y += this->boxSize;

            if (p.position.z > this->boxSize) p.position.z -= this->boxSize;
            else if (p.position.z < 0) p.position.z += this->boxSize;
        }
    }

    // --- Initialization ---
    void initParticles(int count, double mass, double boxSize, double v_max, double epsilon, double sigma) {

        this->boxSize = boxSize;
        this->particles = createParticles(count, mass, boxSize, v_max);
        this->cutoff = boxSize * 0.99;
        this->epsilon = epsilon;
        this->sigma = sigma;

        double sigma2 = this->sigma * this->sigma;
        this->sigma6 = sigma2 * sigma2 * sigma2;
        this->sigma12 = this->sigma6 * this->sigma6;
    }


    // --- Energy / diagnostics ---
    double kineticEnergy() const;
    double potentialEnergy() const;
    double temperature() const;

    // --- Access ---
    std::vector<Particle>& getParticles();
    const std::vector<Particle>& getParticles() const;

    // --- Utilities ---
    int size() const;
};
