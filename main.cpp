#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

#include "object.hpp"
#include "vector.hpp"

Vector get_ground_color(const Vector &ray_origin, const Vector &ray_direction)
{
    const float distance = -ray_origin.y / ray_direction.y;
    const float x = ray_origin.x + ray_direction.x * distance;
    const float z = ray_origin.z + ray_direction.z * distance;

    if ((int)std::abs(std::floor(x)) % 2 ==
        (int)std::abs(std::floor(z)) % 2) {
        return {255, 0, 0};
    }
    else {
        return {255, 255, 255};
    }
}

Vector get_sky_color(const Vector &ray_direction)
{
    return Vector{0.7, 0.6, 1.0} * 255 * std::pow(1 - ray_direction.y, 2);
}

int main()
{
    std::ofstream outfile("outfile.ppm");

    const Vector Z{0, 0, 1};
    const Vector X{0.002, 0, 0};
    const Vector Y{0, 0.002, 0};

    std::vector<Object *> scene_objects;
    scene_objects.push_back(new Triangle{{-1, 0, 0},
                                         {1, 0, 0},
                                         {0, 1.73, 0}});

    int height = 512;
    int width = 512;

    outfile << "P6 " << height << " " << width << " "
            << "255 ";

    for (int y = 256; y >= -255; --y) {
        for (int x = -255; x <= 256; ++x) {

            Vector color{0, 0, 0};
            Vector ray_origin{0, 1, -4};
            Vector ray_direction = Vector{X * (x - 0.5) + Y * (y - 0.5) + Z}.unit();

            float distance_to_hit;
            bool an_object_was_hit{false};
            float min_hit_distance{std::numeric_limits<float>::max()};
            Object *closest_object_ptr{nullptr};

            for (const auto &object : scene_objects) {
                if (object->is_hit_by_ray(ray_origin,
                                          ray_direction,
                                          distance_to_hit,
                                          color)) {
                    an_object_was_hit = true;

                    if (distance_to_hit < min_hit_distance) {
                        min_hit_distance = distance_to_hit;
                        closest_object_ptr = object;
                    }
                }
            }

            if (an_object_was_hit) {
                closest_object_ptr->is_hit_by_ray(ray_origin,
                                                  ray_direction,
                                                  distance_to_hit,
                                                  color);
            }
            else {
                if (ray_direction.y < 0) {
                    color = get_ground_color(ray_origin, ray_direction);
                }
                else {
                    color = get_sky_color(ray_direction);
                }
            }

            outfile << static_cast<unsigned char>(color.x)
                    << static_cast<unsigned char>(color.y)
                    << static_cast<unsigned char>(color.z);
        }
    }

    return EXIT_SUCCESS;
}