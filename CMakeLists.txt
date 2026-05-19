cmake_minimum_required(VERSION 3.16)
project(GasEngine LANGUAGES C CXX)

# Стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Авто-загрузка зависимостей через FetchContent
include(FetchContent)

# GLFW
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.4
        GIT_SHALLOW    ON
)

# Отключаем лишнее в сборке GLFW
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)

# ImGui
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG        v1.91.6
        GIT_SHALLOW    ON
)

FetchContent_MakeAvailable(glfw imgui)

# Целевой исполняемый файл
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

# заголовки
target_include_directories(GasEngine PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

# ImGui: подключаем исходники + бэкенды
target_sources(GasEngine PRIVATE
        # Ядро ImGui
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        # Бэкенды для GLFW + OpenGL 3
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# GLM
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        1.0.1
        GIT_SHALLOW    ON
)

FetchContent_MakeAvailable(glfw imgui glm)

target_include_directories(GasEngine PRIVATE
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)

# Загрузчик OpenGL для ImGui
target_compile_definitions(GasEngine PRIVATE
        IMGUI_IMPL_OPENGL_LOADER_GL3W
)

# Линковка
target_link_libraries(GasEngine PRIVATE
        glfw
        glm::glm
)

# Системные библиотеки по платформам
if(WIN32)
    target_link_libraries(GasEngine PRIVATE
            opengl32
            imm32 ole32 oleaut32 uuid comdlg32 gdi32
    )
elseif(APPLE)
    target_link_libraries(GasEngine PRIVATE
            "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo"
    )
else()
    # Linux / BSD
    find_package(OpenGL REQUIRED)
    target_link_libraries(GasEngine PRIVATE
            OpenGL::GL
            dl pthread
    )
endif()


# Опции компиляции
if(MSVC)
    target_compile_options(GasEngine PRIVATE /W4 /permissive-)
else()
    target_compile_options(GasEngine PRIVATE -Wall -Wextra -Wpedantic)
endif()