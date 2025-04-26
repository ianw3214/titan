#version 400 core
// For this shader, the positions will be the 4 corners of the screen
layout(location = 0) in vec3 aPos;

uniform float screenWidth;
uniform float screenHeight;

uniform float zoom;

out vec3 simulationPos;

void main() {
    simulationPos = vec3(aPos.x * screenWidth / 2.f / zoom, aPos.y * screenHeight / 2.f / zoom, 0.0);
    gl_Position = vec4(aPos, 1.0);
}