#pragma once

#include <vector>
#include "MathCore.hpp"
#include "Particle.hpp"
#include "Shader.hpp"


class Renderer {
private:
    unsigned int VAO;
    unsigned int VBO;

    float pointSize;

public:
    Renderer();
    ~Renderer();

    bool init();
    void renderParticles(const ParticleSystem& system, Shader& shader);

    void setPointSize(float size);
    void shutdown();
};