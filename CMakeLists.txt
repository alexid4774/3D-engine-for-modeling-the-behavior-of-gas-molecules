cmake_minimum_required(VERSION 3.16)
project(GasEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY  https://github.com/ocornut/imgui.git
    GIT_TAG         v1.91.6   
)
FetchContent_MakeAvailable(imgui)


find_package(glfw3 REQUIRED)

add_executable(GasEngine 
    src/gl.c
    src/MathCore.cpp
    src/Particle.cpp
    src/Integrator.cpp
    src/Shader.cpp
    src/Camera.cpp
    src/Renderer.cpp
    src/Application.cpp
    src/main.cpp
)

target_include_directories(GasEngine PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_sources(GasEngine PRIVATE
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(GasEngine PRIVATE
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    C:/msys64/ucrt64/include 
)

target_link_libraries(GasEngine PRIVATE glfw)