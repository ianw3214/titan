#pragma once

#include <chrono>

class HighResolutionClock {
public:
    HighResolutionClock();

    void Tick();

    void Reset();

    double GetDeltaNanoseconds() const;
    double GetDeltaMicroseconds() const;
    double GetDeltaMilliseconds() const;
    double GetDeltaSeconds() const;

    double GetTotalNanoseconds() const;
    double GetTotalMicroseconds() const;
    double GetTotalMilliseconds() const;
    double GetTotalSeconds() const;

private:
    // Initial time point
    std::chrono::high_resolution_clock::time_point m_T0;
    // Time since last tick
    std::chrono::high_resolution_clock::duration m_DeltaTime;
    std::chrono::high_resolution_clock::duration m_TotalTime;
};