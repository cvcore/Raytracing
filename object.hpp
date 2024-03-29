#pragma once

#include "vector.hpp"
#include "helper.hpp"

class Object {
public:
    Object()
        : color(255, 255, 255),
          reflectivity(1),
          diffuse_factor(1),
          hardness_factor(1),
          specular_factor(1),
          roughness_factor(1)
    {
    }

    Vector color;
    float reflectivity;
    float diffuse_factor, hardness_factor, specular_factor;
    float roughness_factor;

    virtual bool is_hit_by_ray(const Vector &incoming_ray_origin,
                               const Vector &incoming_ray_direction,
                               Vector &outgoing_ray_origin,
                               Vector &outgoing_ray_direction,
                               Vector &outgoing_normal,
                               float &hit_distance,
                               Vector &hit_color,
                               float &object_reflectivity,
                               float &object_hardness,
                               float &object_diffuse_factor,
                               float &object_specular_factor) const = 0;

    void set_color(const Vector &v)
    {
        color = v;
    }

    void set_reflectivity(float v)
    {
        reflectivity = v;
    }

    void set_diffuse_factor(float v)
    {
        diffuse_factor = v;
    }

    void set_hardness(float v)
    {
        hardness_factor = v;
    }

    void set_specular_factor(float v)
    {
        specular_factor = v;
    }

    void set_roughness(float v)
    {
        roughness_factor = v;
    }
};

class Triangle : public Object {
public:
    Triangle(const Vector &p0, const Vector &p1, const Vector &p2)
        : p0(p0), p1(p1), p2(p2)
    {
        u = p1 - p0;
        v = p2 - p0;
        normal = v.cross(u);
    }

    bool is_hit_by_ray(const Vector &incoming_ray_origin,
                       const Vector &incoming_ray_direction,
                       Vector &outgoing_ray_origin,
                       Vector &outgoing_ray_direction,
                       Vector &outgoing_normal,
                       float &hit_distance,
                       Vector &hit_color,
                       float &object_reflectivity,
                       float &object_hardness,
                       float &object_diffuse_factor,
                       float &object_specular_factor) const
    {
        if (normal.dot(incoming_ray_direction) > 0)
            return false;

        const float &pox{p0.x};
        const float &poy{p0.y};
        const float &poz{p0.z};
        const float &ux{u.x};
        const float &uy{u.y};
        const float &uz{u.z};
        const float &vx{v.x};
        const float &vy{v.y};
        const float &vz{v.z};
        const float &rx{incoming_ray_direction.x};
        const float &ry{incoming_ray_direction.y};
        const float &rz{incoming_ray_direction.z};
        const float &ox{incoming_ray_origin.x};
        const float &oy{incoming_ray_origin.y};
        const float &oz{incoming_ray_origin.z};

        const float u_factor = (-(ox - pox) * (ry * vz - rz * vy) + (oy - poy) * (rx * vz - rz * vx) - (oz - poz) * (rx * vy - ry * vx)) / (rx * uy * vz - rx * uz * vy - ry * ux * vz + ry * uz * vx + rz * ux * vy - rz * uy * vx);
        const float v_factor = ((ox - pox) * (ry * uz - rz * uy) - (oy - poy) * (rx * uz - rz * ux) + (oz - poz) * (rx * uy - ry * ux)) / (rx * uy * vz - rx * uz * vy - ry * ux * vz + ry * uz * vx + rz * ux * vy - rz * uy * vx);
        const float ray_factor = (-(ox - pox) * (uy * vz - uz * vy) + (oy - poy) * (ux * vz - uz * vx) - (oz - poz) * (ux * vy - uy * vx)) / (rx * uy * vz - rx * uz * vy - ry * ux * vz + ry * uz * vx + rz * ux * vy - rz * uy * vx);

        if (u_factor < 0 or u_factor > 1 or
            v_factor < 0 or v_factor > 1 or
            u_factor + v_factor > 1 or
            ray_factor < 0)
            return false;

        hit_distance = (incoming_ray_direction * ray_factor).norm();
        hit_color = color;

        const Vector n = normal;
        outgoing_ray_origin = p0 + u * u_factor + v * v_factor;
        outgoing_ray_direction = (incoming_ray_direction - n.unit() * (incoming_ray_direction.dot(n.unit())) * 2).unit();
        outgoing_normal = n.unit();

        hit_color = color;
        object_reflectivity = reflectivity;

        object_hardness = hardness_factor;
        object_diffuse_factor = diffuse_factor;
        object_specular_factor = specular_factor;

        return true;
    }

    Vector p0, p1, p2, u, v, normal;
};

class Sphere : public Object {
public:
    Sphere(const Vector &center,
           float radius)
        : center(center),
          radius(radius)
    {
    }

    Vector center;
    float radius;

    bool is_hit_by_ray(const Vector &incoming_ray_origin,
                       const Vector &incoming_ray_direction,
                       Vector &outgoing_ray_origin,
                       Vector &outgoing_ray_direction,
                       Vector &outgoing_normal,
                       float &hit_distance,
                       Vector &hit_color,
                       float &object_reflectivity,
                       float &object_hardness,
                       float &object_diffuse_factor,
                       float &object_specular_factor) const
    {
        const Vector p = center - incoming_ray_origin;
        const float threshold = std::sqrt(p.norm() * p.norm() - radius * radius);
        const float b = p.dot(incoming_ray_direction);

        if (b < threshold) {
            return false;
        }

        const float s = std::sqrt(std::pow(p.norm(), 2) - std::pow(b, 2));
        const float t = std::sqrt(std::pow(radius, 2) - std::pow(s, 2));
        hit_distance = b - t;

        if (hit_distance < 1e-6)
            return false;

        outgoing_ray_origin = incoming_ray_origin + incoming_ray_direction * hit_distance;
        const Vector normal = (-p + incoming_ray_direction * hit_distance).unit();
        outgoing_ray_direction = ((incoming_ray_direction - normal.unit() * (incoming_ray_direction.dot(normal.unit())) * 2).unit()
            + (Vector{random_offset(), random_offset(), random_offset()}.unit() * roughness_factor)).unit();
        outgoing_normal = (outgoing_ray_direction - incoming_ray_direction).unit();

        hit_color = color;
        object_reflectivity = reflectivity;
        outgoing_normal = normal;

        object_hardness = hardness_factor;
        object_diffuse_factor = diffuse_factor;
        object_specular_factor = specular_factor;

        return true;
    }
};