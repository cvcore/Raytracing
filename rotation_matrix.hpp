#pragma once

#include <cmath>

#include "vector.hpp"

class RotationMatrix
{
public:
    RotationMatrix(float x, float y, float z)
    {
        *this = RotationMatrix(1.f, 0.f, 0.f,
                               0.f, std::cos(x), -std::sin(x),
                               0.f, std::sin(x), std::cos(x)) *
                RotationMatrix(std::cos(y), 0.f, std::sin(y),
                               0.f, 1.f, 0.f,
                               -std::sin(y), 0.f, std::cos(y)) *
                RotationMatrix(std::cos(z), -std::sin(z), 0.f,
                               std::sin(z), std::cos(z), 0.f,
                               0.f, 0.f, 1.f);
    }

    RotationMatrix(float a, float b, float c,
                   float d, float e, float f,
                   float g, float h, float i)
    {
        data[0][0] = a; data[0][1] = b; data[0][2] = c;
        data[1][0] = d; data[1][1] = e; data[1][2] = f;
        data[2][0] = g; data[2][1] = h; data[2][2] = i;
    }

    RotationMatrix operator*(const RotationMatrix& R) const
    {
        RotationMatrix result(0, 0, 0, 0, 0, 0, 0, 0, 0);
        for (size_t i = 0; i < 3; ++i)
            for (size_t j = 0; j < 3; ++j)
                for (size_t k = 0; k < 3; ++k) {
                    result.data[i][j] += data[i][k] * R.data[k][j];
                }
        return result;
    }

    Vector operator*(const Vector& v) const
    {
        Vector result;

        result.x = data[0][0] * v.x + data[0][1] * v.y + data[0][2] * v.z;
        result.y = data[1][0] * v.x + data[1][1] * v.y + data[1][2] * v.z;
        result.z = data[2][0] * v.x + data[2][1] * v.y + data[2][2] * v.z;

        return result;
    }

    float data[3][3];
};