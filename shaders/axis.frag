#version 330 core

out vec4 FragColor;

in vec3 fragPos;

uniform vec3 color;
uniform vec3 cameraPos;

void main()
{
    float Length = length(cameraPos - fragPos);
    float depth = Length / 1000.0;
    // FragColor = vec4(color,  depth);
    FragColor = vec4(0.1, 0.1, 0.1, 0.5);
}