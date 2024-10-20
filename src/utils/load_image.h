#ifndef COMPUTER_GRAPHICS_LOAD_IMAGE_H
#define COMPUTER_GRAPHICS_LOAD_IMAGE_H

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

// Load image
unsigned char* loadImage(const char* filepath, int* width, int* height, int* channels) {
    unsigned char* image = stbi_load(filepath, width, height, channels, 0);
    if (!image) {
        std::cerr << "Error loading image\n";
        return nullptr;
    }
    return image;
}

// Save image after modification
void saveImage(const char* filepath, unsigned char* image, int width, int height, int channels) {
    stbi_write_png(filepath, width, height, channels, image, width * channels);
}

#endif //COMPUTER_GRAPHICS_LOAD_IMAGE_H
