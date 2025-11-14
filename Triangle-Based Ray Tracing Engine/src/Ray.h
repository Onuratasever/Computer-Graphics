#ifndef RAH_H
#define RAH_H
#include "Vec3.h"
#include "Ray.h"
class Ray 
{
    public:
        Ray();
        Ray(const Vec3& origin, const Vec3& direction);
        Ray(const Ray& other);
        ~Ray();

        Vec3 getOrigin() const;
        Vec3 getDirection() const;
        Vec3 at(float t) const;

        bool intersectRayWithTriangle(const Vec3& o, const Vec3& d,
                const Vec3& a, const Vec3& b, const Vec3& c,
                float& t, float& beta, float& gamma);
    
    private:
        Vec3 origin;
        Vec3 direction;
};


#endif // RAH_H