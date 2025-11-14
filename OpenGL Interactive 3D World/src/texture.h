#pragma once
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>

inline unsigned int loadTexture(const char* path, bool gamma=false)
{
    unsigned int tex;
    glGenTextures(1,&tex);

    int w,h,nr;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path,&w,&h,&nr,0);
    if(!data){ std::cerr<<"Failed to load tex "<<path<<"\n"; return 0; }

    GLenum format = nr==1 ? GL_RED : (nr==3?GL_RGB:GL_RGBA);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D,0,format,w,h,0,format,GL_UNSIGNED_BYTE,data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    stbi_image_free(data);
    return tex;
}
