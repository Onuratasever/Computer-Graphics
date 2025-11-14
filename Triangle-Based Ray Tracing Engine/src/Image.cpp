#include "Image.h"
#include <iostream>

using namespace std;

Image::Image() : width(0), height(0) {}

Image::Image(int width, int height) : width(width), height(height) 
{
    pixels.resize(width * height);
}

Image::Image(const Image& other) : width(other.width), height(other.height), pixels(other.pixels) {}

Image::~Image() {}

void Image::setPixel(int x, int y, const Color& color) 
{
    if (x >= 0 && x < width && y >= 0 && y < height) 
    {
        //compare old color with new color
        if(pixels[y * width + x].getColorR() == color.getColorR() &&
            pixels[y * width + x].getColorG() == color.getColorG() &&
            pixels[y * width + x].getColorB() == color.getColorB())
        {
            // cout << "Pixel unchanged at (" << x << ", " << y << ")" << endl;
        }
        else
        {
            // cout << "Pixel changed from (" << pixels[y * width + x].getColorR() << ", " << pixels[y * width + x].getColorG() << ", " << pixels[y * width + x].getColorB() << ") to (" 
                // << color.getColorR() << ", " << color.getColorG() << ", " << color.getColorB() << ")" << endl;
        }
        pixels[y * width + x] = color;
    }
}

const Color& Image::getPixel(int x, int y) const 

{
    if (x >= 0 && x < width && y >= 0 && y < height) 
    {
        return pixels[y * width + x];
    }
    
    static Color defaultColor; // Return a default color if out of bounds
    
    return defaultColor;
}

int Image::getWidth() const 
{
    return width;
}

int Image::getHeight() const 
{
    return height;
}

