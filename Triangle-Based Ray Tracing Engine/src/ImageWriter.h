#ifndef ImageWriter_H
#define ImageWriter_H

#include "Image.h"

class ImageWriter {
public:
    ImageWriter();
    ~ImageWriter();

    void writePPM(const char* filename, const Image& image);
    void writePNG(const char* filename, const Image& image);
};

#endif // ImageWriter_H