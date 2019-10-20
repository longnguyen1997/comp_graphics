#ifndef LIGHT_H
#define LIGHT_H

#include <Vector3f.h>

#include "Object3D.h"

#include <limits>

class Light
{
  public:
    virtual ~Light() { }

    // in:  p           is the point to be shaded
    // out: tolight     is direction from p to light source
    // out: intensity   is the illumination intensity (RGB) at point p
    // out: distToLight is absolute distance from P to light (infinity for directional light)
    virtual void getIllumination(const Vector3f &p, 
                                 Vector3f &tolight, 
                                 Vector3f &intensity, 
                                 float &distToLight) const = 0;
};

class DirectionalLight : public Light
{
  public:
    DirectionalLight(const Vector3f &d, const Vector3f &c) :
        _direction(d.normalized()),
        _color(c)
    { }

    virtual void getIllumination(const Vector3f &p,
        Vector3f &tolight,
        Vector3f &intensity,
        float &distToLight) const override;

  private:
    Vector3f _direction;
    Vector3f _color;
};

class PointLight : public Light
{
  public:
    PointLight(const Vector3f &p, const Vector3f &c, float falloff) :
        _position(p),
        _color(c),
        _falloff(falloff)
    { }

    virtual void getIllumination(const Vector3f &p,
        Vector3f &tolight,
        Vector3f &intensity,
        float &distToLight) const override;

  private:
    Vector3f _position;
    Vector3f _color;
    float _falloff;
};

#endif // LIGHT_H
