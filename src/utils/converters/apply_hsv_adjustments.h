#include <vector>
#include <iomanip>
#include "hsv_to_rgb.h"

using std::vector, std::clamp;

void apply_hsv_adjustments(
        unsigned char* image,
        vector<float>& hsvImage,
        int width,
        int height,
        int channels,
        float hueAdjust,
        float saturationAdjust,
        float brightnessAdjust
        ) {

    for (int i = 0; i < width * height; ++i) {
        float h = hsvImage[i * 3] + hueAdjust;
        float s = clamp(hsvImage[i * 3 + 1] + saturationAdjust, 0.0f, 1.0f);
        float v = clamp(hsvImage[i * 3 + 2] + brightnessAdjust, 0.0f, 1.0f);

        float r, g, b;
        hsv_to_rgb(h, s, v, r, g, b);

        image[i * channels] = static_cast<unsigned char>(r * 255);
        image[i * channels + 1] = static_cast<unsigned char>(g * 255);
        image[i * channels + 2] = static_cast<unsigned char>(b * 255);
    }
}
