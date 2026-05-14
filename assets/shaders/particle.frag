#version 330 core

in float vSpeed;
out vec4 FragColor;

void main()
{
    float t = clamp(vSpeed / 10.0, 0.0, 1.0);

    vec3 color;

    if (t < 0.25)
        color = mix(vec3(0.0, 0.0, 1.0), vec3(0.0, 1.0, 1.0), t * 4.0); // blue → cyan
    else if (t < 0.5)
        color = mix(vec3(0.0, 1.0, 1.0), vec3(0.0, 1.0, 0.0), (t - 0.25) * 4.0); // cyan → green
    else if (t < 0.75)
        color = mix(vec3(0.0, 1.0, 0.0), vec3(1.0, 1.0, 0.0), (t - 0.5) * 4.0); // green → yellow
    else
        color = mix(vec3(1.0, 1.0, 0.0), vec3(1.0, 0.0, 0.0), (t - 0.75) * 4.0); // yellow → red

    FragColor = vec4(color, 1.0);
}