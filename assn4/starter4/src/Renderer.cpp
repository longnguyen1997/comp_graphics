#include "Renderer.h"

#include "ArgParser.h"
#include "Camera.h"
#include "Image.h"
#include "Ray.h"
#include "VecUtils.h"

#include <limits>

Renderer::Renderer(const ArgParser &args) :
    _args(args),
    _scene(args.input_file) {
}

void
Renderer::Render() {
    int w = _args.width;
    int h = _args.height;

    Image image(w, h);
    Image nimage(w, h);
    Image dimage(w, h);

    // loop through all the pixels in the image
    // generate all the samples

    // This look generates camera rays and callse traceRay.
    // It also write to the color, normal, and depth images.
    // You should understand what this code does.
    Camera *cam = _scene.getCamera();
    for (int y = 0; y < h; ++y) {
        float ndcy = 2 * (y / (h - 1.0f)) - 1.0f;
        for (int x = 0; x < w; ++x) {
            float ndcx = 2 * (x / (w - 1.0f)) - 1.0f;
            // Use PerspectiveCamera to generate a ray.
            // You should understand what generateRay() does.
            Ray r = cam->generateRay(Vector2f(ndcx, ndcy));

            Hit h;
            Vector3f color = traceRay(r, cam->getTMin(), _args.bounces, h);

            image.setPixel(x, y, color);
            nimage.setPixel(x, y, (h.getNormal() + 1.0f) / 2.0f);
            float range = (_args.depth_max - _args.depth_min);
            if (range) {
                dimage.setPixel(x, y, Vector3f((h.t - _args.depth_min) / range));
            }
        }
    }
    // END SOLN

    // save the files
    if (_args.output_file.size()) {
        image.savePNG(_args.output_file);
    }
    if (_args.depth_file.size()) {
        dimage.savePNG(_args.depth_file);
    }
    if (_args.normals_file.size()) {
        nimage.savePNG(_args.normals_file);
    }
}

Vector3f
Renderer::traceRay(const Ray &r,
                   float tmin,
                   int bounces,
                   Hit &h) const {
    // The starter code only implements basic drawing of sphere primitives.
    // You will implement phong shading, recursive ray tracing, and shadow rays.

    // TODO: IMPLEMENT
    if (_scene.getGroup()->intersect(r, tmin, h)) {
        Vector3f I = _scene.getAmbientLight() * h.getMaterial()->getDiffuseColor();
        Vector3f p = r.pointAtParameter(h.getT());
        for (int i = 0; i < _scene.getNumLights(); ++i) {
            Vector3f tolight;
            Vector3f intensity;
            float distToLight;
            _scene.getLight(i)->getIllumination(p, tolight, intensity, distToLight);
            Vector3f ILight = h.getMaterial()->shade(r, h, tolight, intensity);
            // To compute cast shadows, you will send rays from the surface point to each
            // light source. If an intersection is reported, and the intersection is closer
            // than the distance to the light source, the current surface point is in shadow
            // and direct illumination from that light source is ignored. Note that shadow
            // rays must be sent to all light sources.
            if (_args.shadows) {
                Vector3f shadowRayOrigin = p + 0.05 * tolight;
                Ray shadowRay(shadowRayOrigin, tolight);
                Hit shadowHit = Hit();
                Vector3f shadowTrace = traceRay(shadowRay, 0, 0, shadowHit);
                bool shadowIntersectedSomething = shadowHit.getT() < std::numeric_limits<float>::max();
                float distToIntersection = (shadowRay.pointAtParameter(shadowHit.getT()) - shadowRayOrigin).abs();
                if (
                    shadowIntersectedSomething && distToIntersection < distToLight
                ) {
                    ILight = Vector3f(0); // Object in shadow from this light, discount light.
                }
            }
            I += ILight;
        }
        // Reflections.
        if (bounces > 0) {
            // Recursive call.
            Vector3f V = r.getDirection();
            Vector3f N = h.getNormal().normalized();
            Vector3f R = (V - (2 * Vector3f::dot(V, N) * N)).normalized();
            Hit hPrime = Hit();
            // Add a little epsilon to avoid noise.
            Ray rPrime(p + 0.01 * R, R);
            Vector3f IIndirect = traceRay(rPrime, 0.0f, bounces - 1, hPrime);
            I += (h.getMaterial()->getSpecularColor() * IIndirect);
        }
        return I;
    } else {
        return _scene.getBackgroundColor(r.getDirection());
    };
}

