#include "Application.hpp"

int main() {

    Application app(1280, 720, "Gas Simulation");

    if (!app.init()) {
        return -1;
    }
    app.run();

    return 0;
}