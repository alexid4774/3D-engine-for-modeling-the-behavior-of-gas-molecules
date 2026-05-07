#pragma once

#include <string>
#include <glad/glad.h>

class Shader
{
public:
    // ID шейдерной программы
    unsigned int ID;

    // Конструктор: загружает и компилирует шейдеры
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    // Активировать shader
    void use() const;

    // Uniform функции

    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setMat4(const std::string& name, const float* mat) const;
};