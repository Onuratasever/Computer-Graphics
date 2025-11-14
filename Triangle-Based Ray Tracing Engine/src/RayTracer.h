#ifndef RAYTRACER_H
#define RAYTRACER_H
#include "Ray.h"
#include "Color.h"
#include "Scene.h"

class RayTracer 
{
    public:
        Vec3 computeColorTriangle(const Ray& ray, const Scene& scene, int depth);
        Color RayTracer::computeLighting(const Scene &scene, const Vec3 &hitPoint, const Vec3 &normal,
            const Material &mat, const Ray &ray);
        Color computeAmbientComponent(const Light* ambientLight, const Material& mat) const;
        Color computeReflection(const Scene& scene, const Ray& ray, const Vec3& hitPoint,
                                const Vec3& normal, const Material& mat, const Color& baseColor, int depth) const;
        bool isInShadow(const Scene& scene, const Vec3& origin, const Vec3& direction, float maxDistance) const;
        Color computeReflection(const Scene &scene, const Ray &ray, const Vec3 &hitPoint,
            const Vec3 &normal, const Material &mat, const Color &baseColor, int depth);

    };

#endif // RAYTRACER_H