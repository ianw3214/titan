#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

inline void ThrowIfFailed(HRESULT result)
{
    if (FAILED(result))
    {
        throw std::exception();
    }
}