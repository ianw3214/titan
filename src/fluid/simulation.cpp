#include "simulation.h"

#include <cmath>
#include <random>

#include <glm/gtc/random.hpp>

namespace {

    // =====================================================
    float SmoothingKernel(float radius, float distance)
    {
        constexpr float PI = 3.14159f;
        // float volume = PI * std::pow(radius, 8) / 4.f;
        // float value = std::max(0.f, radius * radius - distance * distance);
        // return value * value * value / volume;
        if (distance >= radius)
        {
            return 0;
        }
        float volume = PI * std::pow(radius, 4) / 6.f;
        return (radius - distance) * (radius - distance) / volume;
    }

    // =====================================================
    float SmoothingKernelDerivative(float radius, float distance)
    {
        constexpr float PI = 3.14159f;
        if (distance >= radius)
        {
            return 0.f;
        }
        // float f = radius * radius - distance * distance;
        // float scale = -24.f / (PI * std::pow(radius, 8));
        // return scale * distance * f * f;
        float scale = 12.f / (std::pow(radius, 4.f) * PI);
        return (distance - radius) * scale;
    }

    // =====================================================
    float ConvertDensityToPressure(float density)
    {
        constexpr float targetDensity = 4.5f;
        constexpr float pressureMultiplier = 0.5f;

        float densityError = density - targetDensity;
        float pressure = densityError * pressureMultiplier;
        return pressure;
    }

    float CalculateSharedPressure(float density, float otherDensity)
    {
        const float pressure1 = ConvertDensityToPressure(density);
        const float pressure2 = ConvertDensityToPressure(otherDensity);
        return (pressure1 + pressure2) / 2.f;
    }

}

// =====================================================
FluidSimulation::FluidSimulation()
    : mLookup(mPositions)
{

}

// =====================================================
void FluidSimulation::SetBounds(float w, float h)
{
    mBoundSize = glm::vec2(w, h);
}

// =====================================================
void FluidSimulation::Initialize(uint16_t numParticles)
{
    constexpr float particleSpacing = 0.05f;

    mPositions.reserve(numParticles);
    mVelocities.reserve(numParticles);
    mDensities.reserve(numParticles);

    uint16_t numParticlesPerRow = static_cast<uint16_t>(std::sqrt(static_cast<float>(numParticles)));
    uint16_t numParticlesPerCol = (numParticles - 1) / numParticlesPerRow + 1;
    float spacing = mParticleSize * 2.f + particleSpacing;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> x_distrib(-mBoundSize.x / 2.f, mBoundSize.x / 2.f);
    std::uniform_real_distribution<> y_distrib(-mBoundSize.y / 2.f, mBoundSize.y / 2.f);

    for (int i = 0; i < numParticles; ++i)
    {
        // const float x = x_distrib(gen);
        // const float y = y_distrib(gen);

        float x = (i % numParticlesPerRow - numParticlesPerRow / 2.f + 0.5f) * spacing;
        float y = (i / numParticlesPerRow - numParticlesPerCol / 2.f + 0.5f) * spacing;
        mPositions.emplace_back(x, y);
        mVelocities.emplace_back(0.f, 0.f);
        mDensities.emplace_back(0.f);
    }
}

// =====================================================
void FluidSimulation::Update(float deltaSeconds)
{
    constexpr float gravity = 981.f;
    constexpr float smoothingRadius = 1.2f;
    mLookup.Update(smoothingRadius);

    for (size_t i = 0; i < mPositions.size(); ++i)
    {
        // mVelocities[i] += glm::vec2(0.f, -1.f) * gravity * deltaSeconds;
        mDensities[i] = CalculateDensity(mPositions[i]);
    }

    for (size_t i = 0; i < mPositions.size(); ++i)
    {
        glm::vec2 pressureForce = CalculatePressureForce(i);
        glm::vec2 pressureAcceleration = pressureForce / mDensities[i];
        if (std::isnan(pressureAcceleration.x) || std::isnan(pressureAcceleration.y))
        {
            __debugbreak();
        }
        mVelocities[i] += pressureAcceleration;
    }

    for (size_t i = 0; i < mPositions.size(); ++i)
    {
        mPositions[i] += mVelocities[i] * deltaSeconds;
        ResolveCollisions(i);
    }
}

// =====================================================
void FluidSimulation::ResolveCollisions(size_t index)
{
    constexpr float dampeningFactor = 0.7f;

    glm::vec2 halfBoundSize = mBoundSize / 2.f - glm::vec2(1.f) * mParticleSize;
    if (std::abs(mPositions[index].x) > halfBoundSize.x)
    {
        mPositions[index].x = halfBoundSize.x * (mPositions[index].x > 0.f ? 1.f : -1.f);
        mVelocities[index].x *= -1.f * dampeningFactor;
    }
    if (std::abs(mPositions[index].y) > halfBoundSize.y)
    {
        mPositions[index].y = halfBoundSize.y * (mPositions[index].y > 0.f ? 1.f : -1.f);
        mVelocities[index].y *= -1.f * dampeningFactor;
    }
}

// =====================================================
float FluidSimulation::CalculateDensity(const glm::vec2& samplePoint)
{
    constexpr float smoothingRadius = 1.2f;
    constexpr float mass = 1.f;

    float density = 0.f;

    // for (const glm::vec2& position : mPositions)
    // {
    //     float distance = glm::length(position - samplePoint);
    //     float influence = SmoothingKernel(smoothingRadius, distance);
    //     density += mass * influence;
    // }
    mLookup.ForEachPointInRadius(samplePoint, smoothingRadius, [this, &density, samplePoint](size_t index){
        const float distance = glm::length(this->mPositions[index] - samplePoint);
        const float influence = SmoothingKernel(smoothingRadius, distance);
        density += mass * influence;
    });

    if (density == 0.f)
    {
        __debugbreak();
    }
    
    return density;
}

// =====================================================
glm::vec2 FluidSimulation::CalculatePressureForce(size_t particleIndex)
{
    constexpr float smoothingRadius = 1.2f;
    constexpr float mass = 1.f;

    glm::vec2 pressureForce = glm::vec2(0.f);

    // for (size_t i = 0; i < mPositions.size(); ++i)
    // {
    //     if (i == particleIndex)
    //     {
    //         continue;
    //     }
    //     float distance = glm::length(mPositions[i] - mPositions[particleIndex]);
    //     glm::vec2 direction = (distance == 0.f) ? glm::circularRand(1.f) : (mPositions[i] - mPositions[particleIndex]) / distance;
    //     float slope = SmoothingKernelDerivative(smoothingRadius, distance);
    //     float density = mDensities[i];
    //     float sharedPressure = CalculateSharedPressure(density, mDensities[particleIndex]);
    //     pressureForce += sharedPressure * direction * slope * mass / density;
    // }
    mLookup.ForEachPointInRadius(mPositions[particleIndex], smoothingRadius, [this, particleIndex, &pressureForce](size_t index){
        if (index == particleIndex)
        {
            return;
        }
        const float distance = glm::length(mPositions[index] - mPositions[particleIndex]);
        const glm::vec2 direction = (distance == 0.f) ? glm::circularRand(1.f) : (mPositions[index] - mPositions[particleIndex]) / distance;
        const float slope = SmoothingKernelDerivative(smoothingRadius, distance);
        const float density = mDensities[index];
        const float sharedPressure = CalculateSharedPressure(density, mDensities[particleIndex]);
        pressureForce += sharedPressure * direction * slope * mass / density;
    });

    if (std::isnan(pressureForce.x) || std::isnan(pressureForce.y))
    {
        __debugbreak();
    }

    return pressureForce;
}