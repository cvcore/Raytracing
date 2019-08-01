#pragma once
#include <cmath>

template <typename T>
inline T clamp(T min_value, T max_value, float value)
{
    return static_cast<T>(std::max<float>(min_value, std::min<float>(value, max_value)));
}