#ifndef COLOR_H
#define COLOR_H

#include <iostream>
#include "Vec3.h"

class Color {
    public:
        Color(): r(0), g(0), b(0) {}
        Color(float r, float g, float b): r(r), g(g), b(b) {}
        Color(const Color& other) : r(other.r), g(other.g), b(other.b) {}

        Color operator+(const Color& other) const;
        Color operator*(float scalar) const;
        Color operator*(Vec3 vec) const;
        Color& operator=(const Color& other);
        Color& operator+=(const Color& other);
        int toInt(float x) const;
        void writeToPPM(std::ostream& out) const;
        float getColorR() const { return r; }
        float getColorG() const { return g; }
        float getColorB() const { return b; }
        
    private:
        float r, g, b;

};


#endif // COLOR_H