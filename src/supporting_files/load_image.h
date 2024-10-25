#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include "iostream"

unsigned char* load_image(const char* filepath, int* width, int* height, int* channels) {
    unsigned char* image = stbi_load(filepath, width, height, channels, 0);
    if (!image) {
        std::cerr << "Error loading image\n";
        return nullptr;
    }
    return image;
}

void save_image(const char* filepath, unsigned char* image, int width, int height, int channels) {
    stbi_write_png(filepath, width, height, channels, image, width * channels);
}
