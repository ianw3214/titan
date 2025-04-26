#version 400 core
// In general, the position values are assumed to be within the range of [-1, 1]
layout(location = 0) in vec3 aPos;

uniform float screenWidth;
uniform float screenHeight;

uniform float zoom;

// Sphere size refers to the radius of the sphere
uniform float sphereSize;

uniform vec3 spherePosition;

out vec3 vertexPosition;

void main() {
    vec3 newPosition = spherePosition + aPos * sphereSize;
    vertexPosition = vec3(newPosition.x, newPosition.y, 0.0);
    gl_Position = vec4(newPosition.x / screenWidth * 2.0 * zoom, newPosition.y / screenHeight * 2.0 * zoom, 0.0, 1.0);
}