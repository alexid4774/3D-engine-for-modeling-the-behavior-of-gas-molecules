#pragma once

#include <string>

struct GLFWwindow;

class Application {
private:
    GLFWwindow* window;
    int width;
    int height;
    std::string title;
    
    bool isRunning;
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