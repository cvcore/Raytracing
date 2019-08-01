#pragma once
#include <cmath>
#include <cstring>

#include "object.hpp"
#include "vector.hpp"

template <typename T>
inline T clamp(T min_value, T max_value, float value)
{
    return static_cast<T>(std::max<float>(min_value, std::min<float>(value, max_value)));
}

inline Vector clamp(float min_value, float max_value, const Vector& vec)
{
    return Vector { std::max(min_value, std::min(max_value, vec.x)),
                    std::max(min_value, std::min(max_value, vec.y)),
                    std::max(min_value, std::min(max_value, vec.z))};
}


bool parse_arguments(int argc, char** argv, bool& write_outfile)
{
    write_outfile = false;
    if (argc > 2) {
        std::cout << "Usage: " << argv[0] << " [-f]\n";
        return false;
    }

    for (int i = 0; i < argc; ++i){
        if (std::strcmp(argv[i], "-f") == 0) {
            write_outfile = true;
        }
    }

    return true;
}

float random_offset()
{
    // Random value in [-0.5, 0.5]
    return static_cast<float>(rand()) / RAND_MAX - 0.5f; 
}