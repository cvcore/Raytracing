#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
//#include <opencv2/opencv.hpp>

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
    const Vector X{0.001, 0, 0};
    const Vector Y{0, 0.001, 0};

    std::vector<Object *> scene_objects;

    scene_objects.push_back(new Triangle{Vector{0, 0, 0},
                                         Vector{-1, 1, 0},
                                         Vector{0, 1, 1}});
    scene_objects.push_back(new Triangle{Vector{0, 0, 0},
                                         Vector{0, 1, -1},
                                         Vector{-1, 1, 0}});
    scene_objects.push_back(new Triangle{Vector{0, 0, 0},
                                         Vector{1, 1, 0},
                                         Vector{0, 1, -1}});
    scene_objects.push_back(new Triangle{Vector{0, 0, 0},
                                         Vector{0, 1, 1},
                                         Vector{1, 1, 0}});
    /// Top half
    scene_objects.push_back(new Triangle{Vector{0, 2, 0},
                                         Vector{0, 1, 1},
                                         Vector{-1, 1, 0}});
    scene_objects.push_back(new Triangle{Vector{0, 2, 0},
                                         Vector{1, 1, 0},
                                         Vector{0, 1, 1}});
    scene_objects.push_back(new Triangle{Vector{0, 2, 0},
                                         Vector{0, 1, -1},
                                         Vector{1, 1, 0}});
    scene_objects.push_back(new Triangle{Vector{0, 2, 0},
                                         Vector{-1, 1, 0},
                                         Vector{0, 1, -1}});

    // scene_objects.push_back(new Triangle{{-2, 0, 1},
    //                                      {2, 0, 1},
    //                                      {0, 3, 1}});
    // scene_objects.back()->set_color({0, 0, 255});
    // scene_objects.back()->set_reflectivity(0.8);
    // scene_objects.push_back(new Triangle{{2, 0, -5},
    //                                      {-2, 0, -5},
    //                                      {0, 3, -5}});
    // scene_objects.back()->set_color({0, 255, 0});
    // scene_objects.back()->set_reflectivity(0.8);

    scene_objects.push_back(new Sphere({1,2,0}, 0.5));
    scene_objects.back()->set_color({0, 0, 0});
    scene_objects.back()->set_reflectivity(0.95);
    scene_objects.push_back(new Sphere({-1.25,0.8,0}, 0.25));
    scene_objects.back()->set_color({255, 0, 0});
    scene_objects.back()->set_reflectivity(0.95);


    int height = 1024;
    int width = 1024;
    const int max_hit_bounces{100};

//    cv::Mat out_image(height, width, CV_8UC3);

    outfile << "P6 " << height << " " << width << " " << "255 ";

    for (int y = height/2; y >= -(height/2 - 1); --y) {
        for (int x = -(width/2 - 1); x <= width/2; ++x) {

            Vector color{0, 0, 0};
            Vector final_color{0, 0, 0};

            Vector ray_origin{0, 1, -4};
            Vector ray_direction = Vector{X * (x - 0.5) + Y * (y - 0.5) + Z}.unit();

            Vector ray_hit_at,
                ray_bounced_direction,
                normal;

            float distance_to_hit;
            float reflectivity_at_hit;
            float ray_energy_left = 1.f;

            for (int bounce = 0; bounce <= max_hit_bounces; ++bounce) {
                bool an_object_was_hit{false};
                float min_hit_distance{std::numeric_limits<float>::max()};
                Object *closest_object_ptr{nullptr};

                for (const auto &object : scene_objects) {
                    if (object->is_hit_by_ray(ray_origin,
                                              ray_direction,
                                              ray_hit_at,
                                              ray_bounced_direction,
                                              distance_to_hit,
                                              color,
                                              reflectivity_at_hit)) {
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
                                                      ray_hit_at,
                                                      ray_bounced_direction,
                                                      distance_to_hit,
                                                      color,
                                                      reflectivity_at_hit);
                    ray_origin = ray_hit_at;
                    ray_direction = ray_bounced_direction;
                }
                else {
                    if (ray_direction.y < 0) {
                        color = get_ground_color(ray_origin, ray_direction);
                        reflectivity_at_hit = 0.f;
                    }
                    else {
                        color = get_sky_color(ray_direction);
                        reflectivity_at_hit = 0.f;
                    }
                }

                final_color = final_color + (color * (ray_energy_left * (1 - reflectivity_at_hit)));
                ray_energy_left *= reflectivity_at_hit;

                if (ray_energy_left <= 0)
                    break;
            }

            outfile << static_cast<unsigned char>(final_color.x)
                    << static_cast<unsigned char>(final_color.y)
                    << static_cast<unsigned char>(final_color.z);
            
        }
    }

    return EXIT_SUCCESS;
}