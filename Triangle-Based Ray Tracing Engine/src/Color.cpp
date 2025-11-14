#include "Color.h"

Color Color::operator+(const Color& other) const 
{
    return Color(r + other.r, g + other.g, b + other.b);
}

Color Color::operator*(float scalar) const
{
    return Color(r*scalar, g*scalar, b*scalar);
}

Color Color::operator*(Vec3 vec) const
{
    return Color(r*vec.x, g*vec.y, b*vec.z);
}

Color& Color::operator=(const Color& other)
{
    if (this != &other) // kendine atamayı engelle
    {
        r = other.r;
        g = other.g;
        b = other.b;
    }
    return *this;
}

Color& Color::operator+=(const Color& other)
{
    r += other.r;
    g += other.g;
    b += other.b;
    return *this;
}

//Clamps the color values to the range [0, 255]
int Color::toInt(float x) const
{
    return std::max(0, std::min(255, static_cast<int>(x * 255.0f)));
}

void Color::writeToPPM(std::ostream& out) const
{
    out << toInt(r) << " " << toInt(g) << " " << toInt(b) << "\n";
}