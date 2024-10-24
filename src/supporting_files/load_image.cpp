
export module load_image;

#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include "iostream"

export unsigned char* load_image(const char* filepath, int* width, int* height, int* channels) {
    unsigned char* image = stbi_load(filepath, width, height, channels, 0);
    if (!image) {
        std::cerr << "Error loading image\n";
        return nullptr;
    }
    return image;
}

export void save_image(const char* filepath, unsigned char* image, int width, int height, int channels) {
    stbi_write_png(filepath, width, height, channels, image, width * channels);
}
