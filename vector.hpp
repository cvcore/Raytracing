#pragma once
#include <cmath>

class Vector {
public:
    union{float x; float r;};
    union{float y; float g;};
    union{float z; float b;};

    Vector(float x = 0.f, float y = 0.f, float z = 0.f):
        x(x), y(y), z(z) {}
    
    Vector(const Vector& rhs):
        x(rhs.x), y(rhs.y), z(rhs.z) {}
    
    Vector operator+(const Vector& rhs) const
    {
        return Vector {x+rhs.x, y+rhs.y, z+rhs.z};
    }

    Vector operator-(const Vector& rhs) const
    {
        return *this + rhs * -1;
    }

    Vector operator-() const
    {
        return *this * -1;
    }

    Vector operator*(float r) const
    {
        return Vector {x*r, y*r, z*r};
    }

    float dot(const Vector& rhs) const
    {
        return x*rhs.x + y*rhs.y + z*rhs.z;
    }

    Vector cross(const Vector& rhs) const
    {
        return Vector {y*rhs.z - z*rhs.y,
                       z*rhs.x - x*rhs.z,
                       x*rhs.y - y*rhs.x};
    }

    float norm() const
    {
        return std::sqrt(x*x + y*y + z*z);
    }

    Vector unit() const
    {
        return (*this) * (1. / norm());
    }
};

