#include "Object3D.h"

bool Sphere::intersect(const Ray &r, float tmin, Hit &h) const {
    // BEGIN STARTER

    // We provide sphere intersection code for you.
    // You should model other intersection implementations after this one.

    // Locate intersection point ( 2 pts )
    const Vector3f &rayOrigin = r.getOrigin(); //Ray origin in the world coordinate
    const Vector3f &dir = r.getDirection();

    Vector3f origin = rayOrigin - _center;      //Ray origin in the sphere coordinate

    float a = dir.absSquared();
    float b = 2 * Vector3f::dot(dir, origin);
    float c = origin.absSquared() - _radius * _radius;

    // no intersection
    if (b * b - 4 * a * c < 0) {
        return false;
    }

    float d = sqrt(b * b - 4 * a * c);

    float tplus = (-b + d) / (2.0f * a);
    float tminus = (-b - d) / (2.0f * a);

    // the two intersections are at the camera back
    if ((tplus < tmin) && (tminus < tmin)) {
        return false;
    }

    float t = 10000;
    // the two intersections are at the camera front
    if (tminus > tmin) {
        t = tminus;
    }

    // one intersection at the front. one at the back
    if ((tplus > tmin) && (tminus < tmin)) {
        t = tplus;
    }

    if (t < h.getT()) {
        Vector3f normal = r.pointAtParameter(t) - _center;
        normal = normal.normalized();
        h.set(t, this->material, normal);
        return true;
    }
    // END STARTER
    return false;
}

// Add object to group
void Group::addObject(Object3D *obj) {
    m_members.push_back(obj);
}

// Return number of objects in group
int Group::getGroupSize() const {
    return (int)m_members.size();
}

bool Group::intersect(const Ray &r, float tmin, Hit &h) const {
    // BEGIN STARTER
    // we implemented this for you
    bool hit = false;
    for (Object3D *o : m_members) {
        if (o->intersect(r, tmin, h)) {
            hit = true;
        }
    }
    return hit;
    // END STARTER
}


Plane::Plane(const Vector3f &normal, float d, Material *m) : Object3D(m) {
    _d = d;
    _normal = normal;
    _m = m;
}

bool Plane::intersect(const Ray &r, float tmin, Hit &h) const {
    // See L9 - Raycasting slides, page 49, for derivation.
    float t = (_d - Vector3f::dot(_normal, r.getOrigin())) / Vector3f::dot(_normal, r.getDirection().normalized());
    if (t > h.getT() || t < tmin) return false;
    h.set(t, _m, _normal);
    return true;
}

bool Triangle::intersect(const Ray &r, float tmin, Hit &h) const {
    // See L10 - Raycasting II slides, page 16.
    Matrix3f A(
        _v[0] - _v[1], _v[0] - _v[2], r.getDirection()
    );
    Vector3f B = _v[0] - r.getOrigin();
    Vector3f X = A.inverse() * B;
    // Barycentric ratios
    float alpha = 1 - X[0] - X[1];
    float beta = X[0];
    float gamma = X[1];
    float t = X[2];
    if (t > h.getT() || t < tmin || alpha < 0 || beta < 0 || gamma < 0) return false;
    h.set(t, material, (alpha * _normals[0] + beta * _normals[1] + gamma * _normals[2]).normalized());
    return true;
}

Transform::Transform(const Matrix4f &m,
                     Object3D *obj) : _object(obj) {
    M = m;
}

bool Transform::intersect(const Ray &r, float tmin, Hit &h) const {

    // Move ray into object coordinate space
    Matrix4f worldToLocal = M.inverse();
    Vector3f rayOriginLocal = (worldToLocal * Vector4f(r.getOrigin(), 1)).xyz();
    Vector3f rayDirectionLocal = (worldToLocal * Vector4f(r.getDirection(), 0)).xyz();
    Ray rLocal = Ray(rayOriginLocal, rayDirectionLocal);

    // Check for intersection.
    if(_object -> intersect(rLocal, tmin, h)) {
        Vector3f normal = (worldToLocal.transposed() * Vector4f(h.getNormal().normalized(), 0)).xyz().normalized();
        h.set(h.getT(), h.getMaterial(), normal);
        return true;
    } else {
        return false;
    }
}