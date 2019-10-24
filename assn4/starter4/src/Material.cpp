#include "Material.h"

float clamp(const Vector3f &L, const Vector3f &N) {
	float dotProd = Vector3f::dot(L, N);
	return dotProd > 0 ? dotProd : 0;
}

Vector3f Material::shade(const Ray &ray,
    const Hit &hit,
    const Vector3f &dirToLight,
    const Vector3f &lightIntensity)
{
    // TODO implement Diffuse and Specular phong terms

	// Diffuse term
	float diffuseShading = clamp(dirToLight, hit.getNormal());
	Vector3f iDiffuse = diffuseShading * lightIntensity * getDiffuseColor();

    return Vector3f(0, 0, 0);
}
