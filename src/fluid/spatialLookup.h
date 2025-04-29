#pragma once

#include <functional>
#include <vector>

#include <glm/vec2.hpp>

// =====================================================
struct LookupEntry {
    // Index into vector of particles
    size_t mIndex = 0;
    // Key corresponding to grid cells in the world
    uint32_t mCellKey = 0;
};

// =====================================================
class SpatialLookup{
public:
    SpatialLookup(const std::vector<glm::vec2>& points);

    void Update(float radius);
    void ForEachPointInRadius(const glm::vec2& target, float radius, std::function<void(size_t)> callback) const;

private:
    uint32_t GetKeyFromHash(uint32_t hash) const;

private:
    const std::vector<glm::vec2>& mPointsRef;
    std::vector<LookupEntry> mEntries;
    std::vector<size_t> mStartIndices;
};