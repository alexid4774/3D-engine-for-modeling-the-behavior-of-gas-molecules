cmake_minimum_required(VERSION 3.16)

project(GasEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(glfw3 REQUIRED)


add_executable(GasEngine 
    src/gl.c
    src/MathCore.cpp
    src/ParticleSystem.cpp
    src/Integrator.cpp
    src/Shader.cpp
    src/Camera.cpp
    src/Renderer.cpp
    src/Application.cpp
    src/main.cpp)


target_include_directories(GasEngine PRIVATE ${CMAKE_SOURCE_DIR}/include)


target_link_libraries(GasEngine PRIVATE glfw)