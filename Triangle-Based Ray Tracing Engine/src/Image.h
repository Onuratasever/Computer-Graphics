// Image.h
#ifndef IMAGE_H
#define IMAGE_H

#include <vector>
#include "Color.h"

class Image{

    public:
        Image();
        Image(int width, int height);
        Image(const Image& other);
        ~Image();

        void setPixel(int x, int y, const Color& color);
        const Color& getPixel(int x, int y) const;

        int getWidth() const;
        int getHeight() const;
    
    private:
        std::vector<Color> pixels;
        int width, height;
};

#endif // IMAGE_H