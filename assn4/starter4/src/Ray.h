#ifndef RAY_H
#define RAY_H

#include "Vector3f.h"

#include <cassert>
#include <limits>
#include <iostream>

// This file defines the Ray and Hit classes

// Ray class mostly copied from Peter Shirley and Keith Morley
class Ray
{
  public:
    Ray(const Vector3f &orig, const Vector3f &dir) :
        _origin(orig),
        _direction(dir)
    {
    }

    const Vector3f getOrigin() const {
        return _origin;
    }

    const Vector3f getDirection() const {
        return _direction;
    }

    Vector3f pointAtParameter(float t) const {
        return _origin + _direction * t;
    }

  private:
    Vector3f _origin;
    Vector3f _direction;

};

inline std::ostream &
operator<<(std::ostream &os, const Ray &r)
{
    os << "Ray <" << r.getOrigin() << ", " << r.getDirection() << ">";
    return os;
}

class Material;
class Hit
{
public:
    // Constructors
    Hit() :
        material(NULL),
        t(std::numeric_limits<float>::max())
    {
    }

    Hit(float argt, Material *argmaterial, const Vector3f &argnormal) :
        t(argt),
        material(argmaterial),
        normal(argnormal)
    {
    }

    float getT() const
    {
        return t;
    }

    Material * getMaterial() const
    {
        return material;
    }

    const Vector3f getNormal() const
    {
        return normal;
    }

    void set(float t, Material *material, const Vector3f &normal)
    {
        this->t = t;
        this->material = material;
        this->normal = normal;
    }

    float     t;
    Material* material;
    Vector3f  normal;
};

inline std::ostream &
operator<<(std::ostream &os, const Hit &h)
{
    os << "Hit <" << h.getT() << ", " << h.getNormal() << ">";
    return os;
}


#endif // RAY_H
