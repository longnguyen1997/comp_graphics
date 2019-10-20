#ifndef VEC_UTILS_H
#define VEC_UTILS_H

#include <vecmath.h>

#include <algorithm>

class VecUtils
{
  public:
    static Vector3f min(const Vector3f &b, const Vector3f &c)
    {
        Vector3f out;

        for (int i = 0; i < 3; ++i) {
            out[i] = std::min(b[i], c[i]);
        }

        return out;
    }

    static Vector3f max(const Vector3f &b, const Vector3f &c)
    {
        Vector3f out;

        for (int i = 0; i < 3; ++i) {
            out[i] = std::max(b[i], c[i]);
        }

        return out;
    }

    static Vector3f clamp(const Vector3f &data, 
                          float low = 0, 
                          float high = 1)
    {
        Vector3f out = data;
        for (int i = 0; i < 3; ++i) {
            if (out[i] < low) {
                out[i] = low;
            }
            if (out[i] > high) {
                out[i] = high;
            }
        }

        return out;
    }

    // transforms a 3D point using a matrix, returning a 3D point
    static Vector3f transformPoint(const Matrix4f &mat, 
                                   const Vector3f &point)
    {
        return (mat * Vector4f(point, 1)).xyz();
    }

    // transform a 3D directino using a matrix, returning a direction
    // This function *does not* take the inverse tranpose for you.
    static Vector3f transformDirection(const Matrix4f &mat, 
                                       const Vector3f &dir)
    {
        return (mat * Vector4f(dir, 0)).xyz();
    }
};

#endif // VEC_UTILS_H
