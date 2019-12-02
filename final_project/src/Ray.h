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
        _orig(orig),
        _dir(dir)
    {
        this->orig = orig;
        this->dir = dir;
        invdir = Vector3f(1.f / dir[0], 1.f / dir[1], 1.f / dir[2]); 
        sign[0] = (invdir.x() < 0); 
        sign[1] = (invdir.y() < 0); 
        sign[2] = (invdir.z() < 0); 
    } 

    const Vector3f getOrigin() const {
        return _orig;
    }

    const Vector3f getDirection() const {
        return _dir;
    }

    Vector3f pointAtParameter(float t) const {
        return _orig + _dir * t;
    }

    Vector3f invdir, _orig, _dir, orig, dir;
    int sign[3];

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
