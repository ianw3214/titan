#version 400 core
out vec4 FragColor;

uniform float sphereSize;

uniform vec3 spherePosition;

in vec3 vertexPosition;

void main() {
    vec3 offset = vertexPosition - spherePosition;
    float distance = length(offset) / sphereSize;
    float alpha = step(0.0, 1.0 - distance);
    FragColor = vec4(1.0, 1.0, 1.0, alpha);
}