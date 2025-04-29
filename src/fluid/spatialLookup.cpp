#include "spatialLookup.h"

#include <algorithm>

#include <glm/glm.hpp>

namespace {

    // =====================================================
    int PositionToCellCoord(float position, float radius)
    {
        return static_cast<int>(position / radius);
    }

    // =====================================================
    uint32_t HashCell(int cellX, int cellY)
    {
        uint32_t a = static_cast<uint32_t>(cellX) * 15823;
        uint32_t b = static_cast<uint32_t>(cellY) * 9737333;
        return a + b;
    }

}

// =====================================================
SpatialLookup::SpatialLookup(const std::vector<glm::vec2>& points)
    : mPointsRef(points)
{

}

// =====================================================
void SpatialLookup::Update(float radius)
{
    if (mEntries.size() != mPointsRef.size())
    {
        mEntries = std::vector<LookupEntry>(mPointsRef.size());
        mStartIndices = std::vector<size_t>(mPointsRef.size());
    }

    if (mEntries.size() == 0 || mEntries.size() != mPointsRef.size())
    {
        return;
    }

    for (size_t index = 0; index < mPointsRef.size(); ++index)
    {
        const int cellX = PositionToCellCoord(mPointsRef[index].x, radius);
        const int cellY = PositionToCellCoord(mPointsRef[index].y, radius);
        const uint32_t cellKey = GetKeyFromHash(HashCell(cellX, cellY));
        mEntries[index] = { index, cellKey };
        mStartIndices[index] = static_cast<size_t>(-1);
    }

    std::sort(mEntries.begin(), mEntries.end(), [](const LookupEntry& a, const LookupEntry& b){
        return a.mCellKey < b.mCellKey;
    });

    for (size_t index = 0; index < mEntries.size(); ++index)
    {
        const uint32_t key = mEntries[index].mCellKey;
        const uint32_t prevKey = index == 0 ? UINT32_MAX : mEntries[index - 1].mCellKey;
        if (key != prevKey)
        {
            mStartIndices[key] = index;
        }
    }
}

// =====================================================
void SpatialLookup::ForEachPointInRadius(const glm::vec2& target, float radius, std::function<void(size_t)> callback) const
{
    const int cellX = PositionToCellCoord(target.x, radius);
    const int cellY = PositionToCellCoord(target.y, radius);
    const float radiusSquared = radius * radius;

    for (int offsetX = -1; offsetX <= 1; offsetX++)
    {
        for (int offsetY = -1; offsetY <= 1; offsetY++)
        {
            const uint32_t key = GetKeyFromHash(HashCell(cellX + offsetX, cellY + offsetY));
            size_t cellStartIndex = mStartIndices[key];

            for (size_t index = cellStartIndex; index < mEntries.size(); ++index)
            {
                if (mEntries[index].mCellKey != key)
                {
                    break;
                }

                const size_t particleIndex = mEntries[index].mIndex;
                const glm::vec2 offset = mPointsRef[particleIndex] - target;
                const float distSquared = glm::dot(offset, offset);

                if (distSquared <= radiusSquared)
                {
                    callback(particleIndex);
                }
            }
        }
    }
}

// =====================================================
uint32_t SpatialLookup::GetKeyFromHash(uint32_t hash) const
{
    return hash % static_cast<uint32_t>(mEntries.size());
}