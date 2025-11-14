#ifndef VEC3_H
#define VEC3_H

#include <cmath>

struct Vec2f {
    float u, v;

    // Skaler çarpma (sağdan)
    Vec2f operator*(float scalar) const
    {
        Vec2f res;
        res.u = u*scalar;
        res.v = v*scalar;

        return res;
    }

    Vec2f operator+(const Vec2f& other) const
    {
        Vec2f res;
        res.u = u + other.u;
        res.v = v + other.v;

        return res;
    }
};

class Vec3 {
    public:
        float x, y, z;
    
        Vec3() : x(0), y(0), z(0) {}
        Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    
        Vec3 operator+(const Vec3& v) const 
        {
            return Vec3(x + v.x, y + v.y, z + v.z);
        }
    
        Vec3 operator+(float scalar)
        {
            return Vec3(scalar + x, scalar + y, scalar + z);
        }

        Vec3 operator-(const Vec3& v) const 
        {
            return Vec3(x - v.x, y - v.y, z - v.z);
        }
        
        Vec3 operator-(float scalar)
        {
            return Vec3(scalar - x, scalar - y, scalar - z);
        }

        Vec3 operator*(float s) const 
        {
            return Vec3(x * s, y * s, z * s);
        }

        Vec3 operator*(Vec3 s) const 
        {
            return Vec3(s.x * x, s.y * y, s.z * z);
            // return Vec3(x * s, y * s, z * s);
        }

        float dot(const Vec3& v) const 
        {
            return x * v.x + y * v.y + z * v.z;
        }
    
        Vec3 cross(const Vec3& v) const 
        {
            return Vec3(
                y * v.z - z * v.y,
                z * v.x - x * v.z,
                x * v.y - y * v.x
            );
        }
    
        float length() const 
        {
            return sqrt(x*x + y*y + z*z);
        }
    
        Vec3 normalized() const 
        {
            float len = length();
            return (len > 0) ? (*this) * (1.0f / len) : Vec3(0, 0, 0);
        }
    };
    
#endif // VEC3_H