#include "../include/Application.hpp"

#include <iostream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

Application::Application(int width, int height, const std::string& title) : window(nullptr), width(width),
      height(height), title(title), isRunning(false) {}

Application::~Application() {
    shutdown();
}

bool Application::init() {
    if (!initGLFW()) {
        return false;
    }

    if (!createWindow()) {
        return false;
    }

    if (!initGLAD()) {
        return false;
    }

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    isRunning = true;
    std::cout << "Application initialized successfully\n";

    return true;
}

bool Application::initGLFW() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    return true;
}

bool Application::createWindow() {
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int newWidth, int newHeight) {
            glViewport(0, 0, newWidth, newHeight);
        }
    );

    return true;
}

bool Application::initGLAD() {
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return false;
    }

    return true;
}

void Application::run() {
    double lastTime = glfwGetTime();
    while (isRunning && !glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double dt = currentTime - lastTime;
        lastTime = currentTime;

        processInput();
        update(dt);
        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Application::processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void Application::update(double dt) {
    // integrator.step(particleSystem, dt);
}

void Application::render() {
    glClearColor(0.02f, 0.02f, 0.04f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // renderer.render(particleSystem);
}

void Application::shutdown() {
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }

    glfwTerminate();
}