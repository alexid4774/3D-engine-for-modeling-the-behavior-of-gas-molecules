#pragma once

#include <string>
#include <glad/gl.h>
#include "MathCore.hpp"

class Shader
{
public:
    unsigned int ID;

public:
    Shader(
        const std::string& vertexPath,
        const std::string& fragmentPath
    );

    void use() const;

    void setFloat(const std::string& name, float value) const;

    void setVec3(const std::string& name,float x,float y,float z) const;

    void setMat4(const std::string& name,const Mat4& mat) const;
};
