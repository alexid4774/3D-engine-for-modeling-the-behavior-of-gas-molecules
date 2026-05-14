#include "Application.hpp"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

Application::Application(int width, int height, const std::string& title)
    : window(nullptr), width(width), height(height),
      title(title), isRunning(false), shader(nullptr),
      particleSystem(nullptr),
      lastMouseX(width / 2.0),
      lastMouseY(height / 2.0),
      firstMouse(true) {
}

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

    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    shader = new Shader(
        "assets/shaders/particle.vert",
        "assets/shaders/particle.frag"
    );

    if (!renderer.init()) {
        std::cerr << "Failed to initialize renderer\n";
        return false;
    }

    particleSystem = new ParticleSystem(
        200,    // count
        1.0f,   // mass
        10.0f,  // boxSize
        10.0f,   // v_max
        0.5f,   // epsilon
        1.0f    // sigma
    );

    particleSystem->computeForces();
    renderer.setPointSize(8.0f);

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

    glfwSetWindowUserPointer(window, this);

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int newWidth, int newHeight) {
        Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        if (app) {
            app->width = newWidth;
            app->height = newHeight;
            glViewport(0, 0, newWidth, newHeight);
        }
    });

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
    Real lastTime = glfwGetTime();
    while (isRunning && !glfwWindowShouldClose(window)) {
        Real currentTime = glfwGetTime();
        Real dt = currentTime - lastTime;
        lastTime = currentTime;

        processInput(static_cast<Real>(dt));
        update(dt);
        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Application::processInput(Real dt) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.moveForward(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.moveBackward(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.moveLeft(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.moveRight(dt);
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera.moveUp(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        camera.moveDown(dt);
    }
}

void Application::update(Real dt) {
    if (particleSystem) {
        integrator.step(*particleSystem, static_cast<Real>(dt));
    }
}

void Application::render()
{
    glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!shader || !particleSystem)
        return;

    Mat4 view = camera.getViewMatrix();
    Mat4 projection = Mat4::perspective(
        Math::PI / 4.0f,
        static_cast<Real>(width) / static_cast<Real>(height),
        0.1f,
        100.0f
    );

    shader->use();

    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    renderer.renderBounds(*shader, 10.0f);

    Mat4 identity = Mat4::identity();
    shader->setMat4("model", identity);

    renderer.renderParticles(*particleSystem, *shader);
}

void Application::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));

    if (!app) return;

    if (app->firstMouse) {
        app->lastMouseX = xpos;
        app->lastMouseY = ypos;
        app->firstMouse = false;
    }

    Real xoffset = static_cast<Real>(xpos - app->lastMouseX);
    Real yoffset = static_cast<Real>(app->lastMouseY - ypos);

    app->lastMouseX = xpos;
    app->lastMouseY = ypos;

    app->camera.processMouseMovement(xoffset, yoffset, true);
}

void Application::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));

    if (!app) return;

    app->camera.moveSpeed += static_cast<Real>(yoffset) * 0.5f;
    if (app->camera.moveSpeed < 1.0f) app->camera.moveSpeed = 1.0f;
    if (app->camera.moveSpeed > 20.0f) app->camera.moveSpeed = 20.0f;
}

void Application::shutdown() {
    renderer.shutdown();

    if (shader) {
        delete shader;
        shader = nullptr;
    }

    if (particleSystem) {
        delete particleSystem;
        particleSystem = nullptr;
    }

    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }

    glfwTerminate();
}
