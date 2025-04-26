#version 400 core
out vec4 FragColor;

uniform float sphereSize;

uniform vec3 spherePosition;

in vec3 vertexPosition;

void main() {
    const float MASS = 1.0;

    vec3 offset = vertexPosition - spherePosition;
    float distance = length(offset);

    const float PI = 3.14159f;
    float volume = PI * pow(sphereSize, 8) / 4.0;
    float value = max(0.0, sphereSize * sphereSize - distance * distance);
    float kernelValue = value * value * value / volume;

    // FragColor = vec4(volume, volume, volume, 1.0);
    FragColor = vec4(1.0, 0.0, 1.0, kernelValue * MASS);
}