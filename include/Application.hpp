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
    float dt;

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

    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void charCallback(GLFWwindow* window, unsigned int c);

public:
    Application(int width, int height, const std::string& title);
    ~Application();

    bool init();
    void run();

    bool free_mouse = false;

    Real previous_time=0.0;
};
