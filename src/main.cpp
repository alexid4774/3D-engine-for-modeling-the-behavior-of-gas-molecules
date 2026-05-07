#include "Application.hpp"
#include "Integrator.hpp"
#include "MathCore.hpp"
#include "Particle.hpp"

int main() {
    Application app(1280, 720, "Gas Molecule Simulation");

    if (!app.init()) {
        return -1;
    }

    app.run();

    return 0;
}