#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <ctime>

#include "object.hpp"
#include "vector.hpp"
#include "helper.hpp"

Vector get_ground_color(const Vector& ray_origin,
                        const Vector& ray_direction,
                        Vector& ray_hit_at)
{
    const float distance = std::abs(ray_origin.y / ray_direction.y);
    const float x = ray_origin.x + ray_direction.x * distance;
    const float z = ray_origin.z + ray_direction.z * distance;

    ray_hit_at = ray_origin + ray_direction * distance;

    if ((int)std::abs(std::floor(x)) % 2 ==
        (int)std::abs(std::floor(z)) % 2) {
        return {255, 0, 0};
    }
    else {
        return {255, 255, 255};
    }
}

Vector get_sky_color(const Vector& ray_direction)
{
    return Vector{0.7, 0.6, 1.0} * 255 * std::pow(1 - ray_direction.y, 2);
}

void generate_objects(std::vector<Object*>& object_container)
{
    object_container.push_back(new Sphere{Vector{1, 2, 0}, .5});
    object_container.back()->set_hardness(99);
    object_container.back()->set_diffuse_factor(0.1);
    object_container.back()->set_specular_factor(0.1);
    object_container.back()->set_reflectivity(0.95);
    object_container.back()->set_color({255, 255, 255});
    object_container.back()->set_roughness(0.75);
    object_container.push_back(new Sphere{Vector{-1.25, .8, 0}, .25});
    object_container.back()->set_diffuse_factor(0.9);
    object_container.back()->set_specular_factor(0.5);
    object_container.back()->set_reflectivity(0.05);
    object_container.back()->set_hardness(99);
    object_container.back()->set_color({255, 165, 0});
    object_container.back()->set_roughness(0.75);

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

int main(int argc, char** argv)
{
    bool write_outfile{false};
    std::ofstream outfile;

    if (not parse_arguments(argc, argv, write_outfile)) {
        return EXIT_FAILURE;
    }

    if (write_outfile) {
        outfile.open("outfile.ppm", std::ofstream::out);
    }

    const Vector Z{0, 0, 1};
    const Vector X{0.001, 0, 0};
    const Vector Y{0, 0.001, 0};

    std::vector<Object*> scene_objects;
    generate_objects(scene_objects);

    int height = 1024;
    int width = 1024;
    const int max_hit_bounces{100};

    cv::Mat out_image(height, width, CV_8UC3);

    outfile << "P6 " << height << " " << width << " "
            << "255 ";

    const clock_t begin_time = std::clock(); 

    const int MAX_FRAMES = 256,
        AA_samples = 1;

    for (int frame_idx = 0; frame_idx < MAX_FRAMES; ++frame_idx) {
        int image_idx = 0;
        float x_sample = random_offset();
        float y_sample = random_offset();
        for (int y = height / 2; y >= -(height / 2 - 1); --y) {
            for (int x = -(width / 2 - 1); x <= width / 2; ++x) {
                Vector aa_color{0, 0, 0};
                for (int sample = 0; sample < AA_samples; ++sample) {

                    Vector color{0, 0, 0};
                    Vector final_color{0, 0, 0};

                    Vector ray_origin{0, 1, -4};
                    Vector ray_direction = Vector{X * (x + random_offset() - 0.5f) + Y * (y + random_offset() - 0.5f) + Z}.unit();

                    Vector ray_hit_at,
                        ray_bounced_direction,
                        normal_at_hit;

                    float distance_to_hit;
                    float reflectivity_at_hit;
                    float ray_energy_left = 1.f;

                    float diffuse_factor_at_hit,
                        specular_factor_at_hit,
                        hardness_factor_at_hit;

                    bool sky_hit{false};

                    for (int bounce = 0; bounce <= max_hit_bounces; ++bounce) {
                        bool an_object_was_hit{false};
                        float min_hit_distance{std::numeric_limits<float>::max()};
                        Object* closest_object_ptr{nullptr};

                        for (const auto& object : scene_objects) {
                            if (object->is_hit_by_ray(ray_origin,
                                                      ray_direction,
                                                      ray_hit_at,
                                                      ray_bounced_direction,
                                                      normal_at_hit,
                                                      distance_to_hit,
                                                      color,
                                                      reflectivity_at_hit,
                                                      hardness_factor_at_hit,
                                                      diffuse_factor_at_hit,
                                                      specular_factor_at_hit)) {

                                if (distance_to_hit < min_hit_distance) {
                                    an_object_was_hit = true;
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
                                                              normal_at_hit,
                                                              distance_to_hit,
                                                              color,
                                                              reflectivity_at_hit,
                                                              hardness_factor_at_hit,
                                                              diffuse_factor_at_hit,
                                                              specular_factor_at_hit);
                            ray_origin = ray_hit_at;
                            ray_direction = ray_bounced_direction;
                        }
                        else {
                            if (ray_direction.y < 0) {
                                color = get_ground_color(ray_origin, ray_direction, ray_hit_at);
                                normal_at_hit = {0, 1, 0};
                                ray_bounced_direction = (ray_direction +
                                                         (normal_at_hit.unit() * (-ray_direction.dot(normal_at_hit.unit()) * 2)).unit());

                                ray_origin = ray_hit_at;
                                ray_direction = ray_bounced_direction;

                                reflectivity_at_hit = 0;
                                diffuse_factor_at_hit = 1;
                                hardness_factor_at_hit = 0;
                                specular_factor_at_hit = 0;
                            }
                            else {
                                color = get_sky_color(ray_direction);
                                reflectivity_at_hit = 0.f;
                                sky_hit = true;
                            }
                        }

                        const int light_size = 3;
                        Vector light_at{10 * std::cosf(7.f * frame_idx / MAX_FRAMES) + random_offset() * light_size, 10, 10 * std::sinf(7.f * frame_idx / MAX_FRAMES) + random_offset() * light_size};
                        Vector light_color{255, 255, 255};
                        // model light source

                        bool point_is_directly_lit{true};
                        for (const auto& object : scene_objects) {
                            // dummy vars
                            Vector v1, v2, v3, v4;
                            float f1, f2, f3, f4, f5;
                            // test occlusion from light source
                            if (object->is_hit_by_ray(ray_hit_at,
                                                      (light_at - ray_hit_at).unit(),
                                                      v1, v2, v3, f1, v4, f2, f3, f4, f5)) {
                                point_is_directly_lit = false;
                                break;
                            }
                        }

                        if (not sky_hit) {
                            const float ambient_light = 0.3;
                            if (point_is_directly_lit) {
                                const float diffuse_light = std::max(0.f, normal_at_hit.dot((light_at - ray_hit_at).unit()));
                                const float specular_light = std::max(0.f, (light_at - ray_hit_at).unit().dot(ray_direction));
                                color = color * (ambient_light + diffuse_light) * diffuse_factor_at_hit +
                                        light_color * std::pow(specular_light, hardness_factor_at_hit) * specular_factor_at_hit;
                            }
                            else {
                                color = color * ambient_light * diffuse_factor_at_hit;
                            }
                        }

                        final_color = final_color + color * ray_energy_left * (1 - reflectivity_at_hit);
                        ray_energy_left *= reflectivity_at_hit;

                        if (ray_energy_left <= 0)
                            break;
                    }

                    final_color = clamp(0, 255, final_color);
                    aa_color = aa_color + final_color * (1. / AA_samples);

                }
                if (write_outfile) {
                    outfile << aa_color.x
                            << aa_color.y
                            << aa_color.z;
                }
                else {
                    out_image.at<unsigned char>(image_idx++) = aa_color.b;
                    out_image.at<unsigned char>(image_idx++) = aa_color.g;
                    out_image.at<unsigned char>(image_idx++) = aa_color.r;
                }
            }
        }

        float time_elapsed = float(std::clock() - begin_time) / CLOCKS_PER_SEC;
        std::cout << "Time elapsed: " << time_elapsed << " FPS: " << 1. / time_elapsed << "\n";

        if (not write_outfile) {
            cv::imshow("Final Render", out_image);
            cv::waitKey(0);
        }
    }

    if (write_outfile) {
        outfile.close();
    }
    return EXIT_SUCCESS;
}