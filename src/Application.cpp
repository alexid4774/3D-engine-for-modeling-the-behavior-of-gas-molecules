#include "Application.hpp"

#include <glad/gl.h>
#include <GLFW/glfw3.h>


Application::Application(int width, int height, const std::string& title) : window(nullptr), width(width), height(height),
      title(title), isRunning(false), shader(nullptr), particleSystem(nullptr) {}


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

    shader = new Shader(
    "assets/shaders/particle.vert",
    "assets/shaders/particle.frag");

    if (!renderer.init()) {
        std::cerr << "Failed to initialize renderer\n";
        return false;
    }

    particleSystem = new ParticleSystem(
        200,    // count
        1.0f,   // mass
        10.0f,  // boxSize
        1.0f,   // v_max
        1.0f,   // epsilon
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
    if (particleSystem) {
        integrator.step(*particleSystem, static_cast<float>(dt));
    }
}


void Application::render() {
    glClearColor(0.02f, 0.02f, 0.04f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!shader || !particleSystem) {
        return;
    }

    Mat4 model = Mat4::identity();

    Mat4 view = camera.getViewMatrix();

    Mat4 projection = Mat4::perspective(
        Math::PI / 4.0f,
        static_cast<Real>(width) / static_cast<Real>(height),
        0.1f,
        100.0f
    );

    shader->use();
    shader->setMat4("model", model);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setVec3("particleColor", 1.0f, 0.0f, 0.0f);

    renderer.renderParticles(*particleSystem, *shader);
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