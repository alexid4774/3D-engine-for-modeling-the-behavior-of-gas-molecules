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

    Real lastMouseX;
    Real lastMouseY;
    bool firstMouse;

private:
    bool initGLFW();
    bool createWindow();
    bool initGLAD();

    void processInput(Real dt);
    void update(Real dt);
    void render();

    void shutdown();

    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

public:
    Application(int width, int height, const std::string& title);
    ~Application();

    bool init();
    void run();
};
