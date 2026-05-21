#include "Application.hpp"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


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
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, false);
    ImGui_ImplOpenGL3_Init("#version 330");
    isRunning = true;
    std::cout << "Application initialized successfully\n";

    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCharCallback(window, charCallback);
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
        40,    // count
        1.0f,   // mass
        10.0f,  // boxSize
        10.0f,   // v_max
        0.5f,   // epsilon
        1.0f,    // sigma
        1        //integratorMode    
    );

    particleSystem->computeForces();
    renderer.setPointSize(8.0f);
    dt = 0.01;

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
    this->previous_time = glfwGetTime();
    while (isRunning && !glfwWindowShouldClose(window)) {
        Real lastTime = glfwGetTime();
        Real dt_old = lastTime - this->previous_time;
        processInput(static_cast<Real>(dt_old));
        
        if (this->particleSystem->size() > 100 || this->particleSystem->getVmax() > 10) {
            update(dt);
        }
        
        else
            update(dt_old);
        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
        this->previous_time = lastTime;
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

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        static double lastCPress = 0.0;
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && glfwGetTime() - lastCPress > 0.3) {
        Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        app->free_mouse = !app->free_mouse;
        glfwSetInputMode(window, GLFW_CURSOR, app->free_mouse ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        if (!app->free_mouse) {
            app->firstMouse = true;  
        }
        lastCPress = glfwGetTime();
        }
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
    glFinish();
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

    renderer.renderBounds(*shader, particleSystem->getBoxSize());

    Mat4 identity = Mat4::identity();
    shader->setMat4("model", identity);

    renderer.renderParticles(*particleSystem, *shader);


    if (free_mouse) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(0.7f);  
        ImGui::Begin("##Controls", nullptr,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_AlwaysAutoResize);

        static int particleCount = 40;
        static float v_max = 10.0f;
        static float boxSize = 10.0f;
        static int integratorMode = 1;

        ImGui::InputInt("Parcicle count", &particleCount);
        ImGui::InputFloat("v_max ", &v_max);
        ImGui::InputFloat("BoxSize", &boxSize);

        ImGui::RadioButton("Euler", &integratorMode, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Verlet", &integratorMode, 1);
        ImGui::SameLine();
        ImGui::RadioButton("LeapFrog", &integratorMode, 2);

        static float moleculeScale = 12.0f;
        ImGui::SliderFloat("Molecule Size", &moleculeScale, 1.0f, 30.0f);
        renderer.setPointSize(moleculeScale);

        if (ImGui::Button("Reboot")) {
            particleSystem->initParticles(particleCount, 1.0f, boxSize, v_max, 0.5f, 1.0f, integratorMode);
            particleSystem->computeForces();
            float vmax = 0.0f;

            for (auto& p : this->particleSystem->getParticles()) {
                float v = p.velocity.length();
                if (v > vmax) vmax = v;
            }

            this->dt = 0.05f * this->particleSystem->getSigma() / (vmax + 1e-6f);
        }

        ImGui::Text("Kinetic: %.3f", particleSystem->kineticEnergy());
        ImGui::Text("Potential: %.3f", particleSystem->potentialEnergy());
        ImGui::Text("Temp: %.3f", particleSystem->temperature());

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}

void Application::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    
    if (!app) return;

    if (app->free_mouse) return;

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


void Application::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (app && app->free_mouse) {
        ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    }
}

void Application::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (app && app->free_mouse) {
        ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    }
}

void Application::charCallback(GLFWwindow* window, unsigned int c) {
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (app && app->free_mouse) {
        ImGui_ImplGlfw_CharCallback(window, c);
    }
}

void Application::shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
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
