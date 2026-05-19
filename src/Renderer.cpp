#include "Renderer.hpp"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

float cubeVertices[] = {
    // нижняя грань
    0, 0, 0,
    1, 0, 0,
    1, 1, 0,
    0, 1, 0,

    // верхняя грань
    0, 0, 1,
    1, 0, 1,
    1, 1, 1,
    0, 1, 1
};

unsigned int cubeIndices[] = {
    0,1, 1,2, 2,3, 3,0,
    4,5, 5,6, 6,7, 7,4,
    0,4, 1,5, 2,6, 3,7
};

Renderer::Renderer() : VAO(0), VBO(0), pointSize(5.0f) {}

Renderer::~Renderer() {
    shutdown();
}

bool Renderer::init()
{

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),
        (void*)0
    );
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1,
        1,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),
        (void*)(3 * sizeof(float))
    );
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);


    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(cubeVertices),
        cubeVertices,
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(cubeIndices),
        cubeIndices,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        (void*)0
    );
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);

    return true;
}

void Renderer::renderBounds(Shader& shader, float boxSize)
{
    shader.use();

    glBindVertexArray(cubeVAO);

    Mat4 model = Mat4::translation(Vec3(-boxSize * 0.5f, -boxSize * 0.5f, -boxSize * 0.5f)) * Mat4::scale(Vec3(boxSize, boxSize, boxSize));

    shader.setMat4("model", model);

    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);

    shader.setFloat("pointScale", pointSize);

    glDrawArrays(GL_POINTS, 0, particles.size());

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