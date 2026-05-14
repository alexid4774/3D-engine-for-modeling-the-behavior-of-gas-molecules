#pragma once

#include <string>

#include "Shader.hpp"
#include "Camera.hpp"
#include "Renderer.hpp"
#include "Particle.hpp"
#include "Integrator.hpp"

struct GLFWwindow;

class Application {
private:
    GLFWwindow* window;

    int width;
    int height;
    std::string title;

    bool isRunning;

    Shader* shader;
    Camera camera;
    Renderer renderer;
    ParticleSystem* particleSystem;
    VelocityVerletIntegrator integrator;

private:
    bool initGLFW();
    bool createWindow();
    bool initGLAD();

    void processInput();
    void update(double dt);
    void render();

    void shutdown();

public:
    Application(int width, int height, const std::string& title);
    ~Application();

    bool init();
    void run();
};