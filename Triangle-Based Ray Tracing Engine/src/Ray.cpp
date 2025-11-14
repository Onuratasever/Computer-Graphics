#include "Ray.h"
#include <iostream>

using namespace std;

Ray::Ray() : origin(Vec3()), direction(Vec3()) {}

Ray::Ray(const Vec3& origin, const Vec3& direction) : origin(origin), direction(direction) {}

Ray::Ray(const Ray& other) : origin(other.origin), direction(other.direction) {}

Ray::~Ray() {}

Vec3 Ray::getOrigin() const 
{
    return origin;
}

Vec3 Ray::getDirection() const 
{
    return direction;
}

Vec3 Ray::at(float t) const 
{
    // cout << "Direction: " << direction.x << " " << direction.y << " " << direction.z << endl;
    return origin + direction*t;
}

bool Ray::intersectRayWithTriangle(const Vec3& o, const Vec3& d,
    const Vec3& a, const Vec3& b, const Vec3& c,
    float& t, float& beta, float& gamma)
{
    Vec3 e1 = b - a;
    Vec3 e2 = c - a;

    Vec3 s = o - a;

    // Matrisin columns:
    // A = [ -d | e1 | e2 ] → 3x3 matrix
    Vec3 col1 = d*-1; // -d → Işın yönü ters çevriliyor
    Vec3 col2 = e1;
    Vec3 col3 = e2;

    // det(A) → determinant calc: |A| = col1 · (col2 × col3)
    float det = col1.dot(col2.cross(col3)); // cross product ile çarpım

    // if determinant too close to zero, return false
    if (fabs(det) < 1e-8) return false;

    // inverse det
    float invDet = 1.0f / det;

    // Cramer’s Rule:
    // t = |[s | e1 | e2]| / |A| = dot(s, (e1 × e2)) / det
    float tVal = s.dot(col2.cross(col3)) * invDet; // t = |[s | e1 | e2]| / |A|

    // β = |[-d | s | e2]| / |A| = dot(-d, (s × e2)) / det
    // float betaVal = dot(col1, cross(s, col3)) * invDet;
    float betaVal = col1.dot(s.cross(col3)) * invDet; // β = |[-d | s | e2]| / |A|

    // γ = |[-d | e1 | s]| / |A| = dot(-d, (e1 × s)) / det
    // float gammaVal = dot(col1, cross(col2, s)) * invDet;
    float gammaVal = col1.dot(col2.cross(s)) * invDet; // γ = |[-d | e1 | s]| / |A|
    // Barycentric conditions:
    if (betaVal < 0 || gammaVal < 0 || (betaVal + gammaVal) > 1) return false;

    // t < 0
    if (tVal < 0) return false;

    t = tVal;
    beta = betaVal;
    gamma = gammaVal;

    return true;
}