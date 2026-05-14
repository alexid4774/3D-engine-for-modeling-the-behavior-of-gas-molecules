#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in float aSpeed;

out float vSpeed;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float pointScale;

void main()
{
    vSpeed = aSpeed;

    gl_Position = projection * view * model * vec4(aPos, 1.0);

    gl_PointSize = pointScale * (vSpeed / 10.0 + 0.2);
}