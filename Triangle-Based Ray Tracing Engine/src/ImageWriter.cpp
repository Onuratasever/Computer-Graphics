#include "ImageWriter.h"
#include <iostream>
#include <fstream>

ImageWriter::ImageWriter() = default;
ImageWriter::~ImageWriter() = default;

void ImageWriter::writePPM(const char* filename, const Image& image)
{
    std::ofstream out(filename);

    if(!out)
    {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    // PPM Title
    out << "P3\n";
    out << image.getWidth() << " " << image.getHeight() << "\n";
    out << "255\n";

    // Piksel data
    for (int y = 0; y < image.getHeight(); ++y) 
    {
        for (int x = 0; x < image.getWidth(); ++x) 
        {
            const Color& c = image.getPixel(x, y);
            
            out << c.toInt(c.getColorR()) << " "
                << c.toInt(c.getColorG()) << " "
                << c.toInt(c.getColorB()) << "\n";
        }
    }

    out.close();
    std::cout << "PPM is written: " << filename << "\n";
}
