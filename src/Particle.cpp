#include <vector>
#include <random>
#include "MathCore.hpp"
#include "Integrator.hpp"
#include "Particle.hpp"


std::mt19937 gen(42);


Particle::Particle() : position(0,0,0), velocity(0,0,0), force(0,0,0), mass(1.0) {}


Particle createParticle(Vec3 position, Vec3 velocity, float mass) {

    Particle p;

    p.position = position;
    p.velocity = velocity;
    p.mass = mass;
    p.force = Vec3(0, 0, 0);

    return p;
}


Vec3 randomPosition(float boxSize) {
    std::uniform_real_distribution<float> dist(0, boxSize);

    float half = boxSize * 0.5f;

    return Vec3(dist(gen) - half, dist(gen) - half, dist(gen) - half);
}


Vec3 randomVelocity(float sigma) {
    std::normal_distribution<float> dist(0.0f, sigma);

    return Vec3(dist(gen), dist(gen), dist(gen));
}


std::vector<Particle> createParticles(int count, float mass, float boxSize, float v_max) {
    std::vector<Particle> particles;
    particles.reserve(count);

    for (int i = 0; i < count; i++) {

        Vec3 position = randomPosition(boxSize);
        Vec3 velocity = randomVelocity(v_max / 3);

        particles.push_back(createParticle(position, velocity, mass));
    }

    return particles;
}



ParticleSystem::ParticleSystem(int count, float mass, float boxSize, float v_max, float epsilon, float sigma, int integratorMode) {
    this->initParticles(count, mass, boxSize, v_max, epsilon, sigma, integratorMode);
}

void ParticleSystem::resetForces() {
    for (auto& p : this->particles)
        p.force = Vec3(0,0,0);
}

void ParticleSystem::computeForces() {

    resetForces();

    float cutoff2 = this->cutoff * this->cutoff;


    for (int i = 0; i < this->particles.size(); i++) {
        for (int j = i + 1; j < this->particles.size(); j++) {

            Vec3 r = computeSeparation(i, j);

            float r2 = r.lengthSq();

            if (r2 > cutoff2 || r2 < 1e-12) continue;

            float inv_r2 = 1.0f / r2;
            float inv_r6 = inv_r2 * inv_r2 * inv_r2;
            float inv_r12 = inv_r6 * inv_r6;

            float forceScalar =
                24.0f * this->epsilon *
                (2.0f * this->sigma12 * inv_r12 - this->sigma6 * inv_r6) * inv_r2;

            Vec3 f = r * forceScalar;

            particles[i].force += f;
            particles[j].force -= f;
        }
    }
}


void ParticleSystem::integrate(float dt) {
    if (this->integratorMode == 0) {
        EulerIntegrator v;
        v.step(*this, dt);
    } else if (this->integratorMode == 1) {
        VelocityVerletIntegrator v;
        v.step(*this, dt);
    } else if (this->integratorMode == 2) {
        LeapfrogIntegrator v;
        v.step(*this, dt);
    }
}


void ParticleSystem::applyBoundaries() {
    for (auto& p : this->particles) {

        float half = boxSize * 0.5f;

        p.position.x = std::fmod(p.position.x + half, boxSize);
        if (p.position.x < 0) p.position.x += boxSize;
        p.position.x -= half;

        p.position.y = std::fmod(p.position.y + half, boxSize);
        if (p.position.y < 0) p.position.y += boxSize;
        p.position.y -= half;

        p.position.z = std::fmod(p.position.z + half, boxSize);
        if (p.position.z < 0) p.position.z += boxSize;
        p.position.z -= half;
    }
}

void ParticleSystem::initParticles(int count, float mass, float boxSize, float v_max, float epsilon, float sigma, int integratorMode) {

    gen.seed(42);
    
    this->boxSize = boxSize;
    this->particles = createParticles(count, mass, boxSize, v_max);
    this->cutoff = 2.5f * sigma;
    this->epsilon = epsilon;
    this->sigma = sigma;
    this->mass = mass;

    float sigma2 = this->sigma * this->sigma;
    this->sigma6 = sigma2 * sigma2 * sigma2;
    this->sigma12 = this->sigma6 * this->sigma6;

    float inv_rcut2 = 1.0f / (this->cutoff * this->cutoff);
    float inv_rcut6 = inv_rcut2 * inv_rcut2 * inv_rcut2;
    float inv_rcut12 = inv_rcut6 * inv_rcut6;

    this->U_cut = (this->sigma12 * inv_rcut12 - this->sigma6 * inv_rcut6);

    Vec3 v_avg = computeAverageVelocity();

    for (auto &p: this->particles) {
        p.velocity -= v_avg;
    }

    this->integratorMode = integratorMode;
}

Vec3 ParticleSystem::computeAverageVelocity() const {

    Vec3 velocity_avg(0, 0, 0);

    for (auto &p: this->particles) {
        velocity_avg += p.velocity;
    }

    return velocity_avg / this->particles.size();
}

Vec3 ParticleSystem::computeSeparation(int i, int j) const {

    Vec3 r = this->particles[i].position - this->particles[j].position;

    if (r.x >  0.5 * this->boxSize) r.x -= this->boxSize;
    if (r.x < -0.5 * this->boxSize) r.x += this->boxSize;

    if (r.y >  0.5 * this->boxSize) r.y -= this->boxSize;
    if (r.y < -0.5 * this->boxSize) r.y += this->boxSize;

    if (r.z >  0.5 * this->boxSize) r.z -= this->boxSize;
    if (r.z < -0.5 * this->boxSize) r.z += this->boxSize;

    return r;
}


float ParticleSystem::kineticEnergy() const {
    float velocity_total_Sq = 0;

    for (auto &p: this->particles) {
        velocity_total_Sq += p.velocity.lengthSq();
    }

    return velocity_total_Sq * this->mass * 0.5f;
}


float ParticleSystem::potentialEnergy() const {

    float cutoff2 = this->cutoff * this->cutoff;
    float U_p = 0;

    for (int i = 0; i < this->particles.size(); i++) {
        for (int j = i + 1; j < this->particles.size(); j++) {

            Vec3 r = computeSeparation(i, j);

            float r2 = r.lengthSq();

            if (r2 > cutoff2 || r2 < 1e-12) continue;

            float inv_r2 = 1.0f / r2;
            float inv_r6 = inv_r2 * inv_r2 * inv_r2;
            float inv_r12 = inv_r6 * inv_r6;


            U_p += this->sigma12 * inv_r12 - this->sigma6 * inv_r6 - this->U_cut;

        }
    }

    return 4.0f * this->epsilon * U_p;
}

float ParticleSystem::temperature() const {
    return 2.0f / 3.0f * this->kineticEnergy() / static_cast<float>(this->particles.size() - 1);
}

std::vector<Particle>&  ParticleSystem::getParticles() {
    return this->particles;
}

const std::vector<Particle>&  ParticleSystem::getParticles() const {
    return this->particles;
}

int ParticleSystem::size() const {
    return particles.size();
}
