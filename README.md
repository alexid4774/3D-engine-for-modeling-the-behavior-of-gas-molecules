# Molecular Gas Simulation Engine

3D molecular dynamics simulation using the Lennard-Jones potential.

---

## Features

- Lennard-Jones interactions
- Velocity Verlet integration
- Periodic boundary conditions
- OpenGL visualization
- Real-time simulation
- Camera system
- Custom math library

---

## Technologies

- C++17
- OpenGL 3.3
- GLFW
- GLAD

---

## Simulation

The engine simulates particles interacting through the Lennard-Jones potential:
U(r) = 4ε[(σ/r)^12 - (σ/r)^6]

---

## Project Structure

- include/
- src/
- assets/
- build/

---

## Team

Sidoryak Alexander, Poskrebyshev Vsevolod, Samarov Vladimir

---

## Build

```bash
cmake -S . -B build
cmake --build build
