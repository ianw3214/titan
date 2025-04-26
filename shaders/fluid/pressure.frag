#version 400 core
out vec4 FragColor;

uniform vec3 particles[121];

in vec3 simulationPos;

// =====================================================
float SmoothingKernel(float radius, float distance)
{
    const float PI = 3.14159;
    // float volume = PI * pow(radius, 8) / 4.0;
    // float value = max(0.0, radius * radius - distance * distance);
    // return value * value * value / volume;
    if (distance >= radius)
    {
        return 0.0;
    }
    float volume = PI * pow(radius, 4) / 6.0;
    return (radius - distance) * (radius - distance) / volume;
}

// =====================================================
float ConvertDensityToPressure(float density)
{
    const float targetDensity = 1.0;
    const float pressureMultiplier = 0.5;

    float densityError = density - targetDensity;
    float pressure = densityError * pressureMultiplier;
    return pressure;
}

// =====================================================
float getPressureAtFragment() {
    const float smoothingRadius = 1.2;
    const float mass = 1.0;

    float density = 0.0;
    for (int i = 0; i < 121; ++i)
    {
        float distance = length(particles[i] - simulationPos);
        float influence = SmoothingKernel(smoothingRadius, distance);
        density += mass * influence;
    }
    return ConvertDensityToPressure(density);
}

// =====================================================
void main() {
    float pressure = getPressureAtFragment();
    vec3 blue = mix(vec3(1.0, 1.0, 1.0), vec3(0.0, 0.0, 1.0), -min(0.0, pressure));
    vec3 red = mix(blue, vec3(1.0, 0.0, 0.0), max(0.0, pressure));
    FragColor = vec4(red, 1.0);
}