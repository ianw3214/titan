#pragma once

#include <vector>

#include <glm/vec2.hpp>

// =====================================================
class FluidSimulation {
public:
    void SetBounds(float w, float h);

    void Initialize(uint16_t numParticles);
    void Update(float deltaSeconds);

    const std::vector<glm::vec2>& GetPositions() const { return mPositions; }

private:
    void ResolveCollisions(size_t index);
    float CalculateDensity(const glm::vec2& samplePoint);
    glm::vec2 CalculatePressureForce(size_t particleIndex);

private:
    // Simulation settings
    float mParticleSize = 0.1f;
    glm::vec2 mBoundSize = glm::vec2(0.f, 0.f);

    std::vector<glm::vec2> mPositions;
    std::vector<glm::vec2> mVelocities;
    std::vector<float> mDensities;
};