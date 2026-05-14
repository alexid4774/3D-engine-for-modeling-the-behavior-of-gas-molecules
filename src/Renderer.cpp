#include "Renderer.hpp"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

Renderer::Renderer() : VAO(0), VBO(0), pointSize(5.0f) {}

Renderer::~Renderer() {
    shutdown();
}

bool Renderer::init() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // скорость
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return true;
}

void Renderer::renderParticles(const ParticleSystem& system, Shader& shader) {
    const std::vector<Particle>& particles = system.getParticles();

    std::vector<float> data;
    data.reserve(particles.size() * 4);

    for (const auto& p : particles) {
        float speed = p.velocity.length();

        data.push_back(static_cast<float>(p.position.x));
        data.push_back(static_cast<float>(p.position.y));
        data.push_back(static_cast<float>(p.position.z));
        data.push_back(speed);
    }

    shader.use();
    glPointSize(pointSize);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);
    
    shader.use();

    shader.setFloat("pointScale", pointSize);
    glDrawArrays(GL_POINTS, 0, static_cast<int>(particles.size()));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::setPointSize(float size) {
    pointSize = size;
}

void Renderer::shutdown() {
    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
}