#pragma once
#include <cmath>
#include <cstring>

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

void generate_objects(std::vector<Object*>& object_container)
{
    object_container.push_back(new Sphere{Vector{1, 2, 0}, .5});
    object_container.back()->set_hardness(99);
    object_container.back()->set_diffuse_factor(0.1);
    object_container.back()->set_specular_factor(0.1);
    object_container.back()->set_reflectivity(0.95);
    object_container.back()->set_color({255, 255, 255});
    object_container.push_back(new Sphere{Vector{-1.25, .8, 0}, .25});
    object_container.back()->set_diffuse_factor(0.9);
    object_container.back()->set_specular_factor(0.5);
    object_container.back()->set_reflectivity(0.05);
    object_container.back()->set_hardness(99);
    object_container.back()->set_color({255, 165, 0});

    object_container.push_back(new Triangle{Vector{0, 0, 0},
                                         Vector{-1, 1, 0},
                                         Vector{0, 1, 1}});
    object_container.back()->set_diffuse_factor(0);
    object_container.push_back(new Triangle{Vector{0, 0, 0},
                                         Vector{0, 1, -1},
                                         Vector{-1, 1, 0}});
    object_container.back()->set_diffuse_factor(0);
    object_container.push_back(new Triangle{Vector{0, 0, 0},
                                         Vector{1, 1, 0},
                                         Vector{0, 1, -1}});
    object_container.back()->set_diffuse_factor(0);
    object_container.push_back(new Triangle{Vector{0, 0, 0},
                                         Vector{0, 1, 1},
                                         Vector{1, 1, 0}});
    object_container.back()->set_diffuse_factor(0);
    object_container.push_back(new Triangle{Vector{0, 2, 0},
                                         Vector{0, 1, 1},
                                         Vector{-1, 1, 0}});
    object_container.back()->set_diffuse_factor(0);
    object_container.push_back(new Triangle{Vector{0, 2, 0},
                                         Vector{1, 1, 0},
                                         Vector{0, 1, 1}});
    object_container.back()->set_diffuse_factor(0);
    object_container.push_back(new Triangle{Vector{0, 2, 0},
                                         Vector{0, 1, -1},
                                         Vector{1, 1, 0}});
    object_container.back()->set_diffuse_factor(0);
    object_container.push_back(new Triangle{Vector{0, 2, 0},
                                         Vector{-1, 1, 0},
                                         Vector{0, 1, -1}});
    object_container.back()->set_diffuse_factor(0);
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