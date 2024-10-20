#ifndef COMPUTER_GRAPHICS_CONVERTERS_H
#define COMPUTER_GRAPHICS_CONVERTERS_H
#include "../../includes.h"

void rgb_to_hsv(float r, float g, float b, float &h, float &s, float &v) {
    float max = std::max(r, std::max(g, b));
    float min = std::min(r, std::min(g, b));
    v = max;

    float delta = max - min;
    if (delta < 0.00001f) {
        h = 0.0f;
        s = 0.0f;
        return;
    }

    s = (max > 0.0f) ? (delta / max) : 0.0f;

    if (r >= max) {
        h = (g - b) / delta;
    } else if (g >= max) {
        h = 2.0f + (b - r) / delta;
    } else {
        h = 4.0f + (r - g) / delta;
    }

    h *= 60.0f;
    if (h < 0.0f) {
        h += 360.0f;
    }
}

void hsv_to_rgb(float h, float s, float v, float &r, float &g, float &b) {
    if (s <= 0.0f) {
        r = g = b = v;
        return;
    }

    float hh = h;
    if (hh >= 360.0f) hh = 0.0f;
    hh /= 60.0f;
    int i = (int)hh;
    float ff = hh - i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - (s * ff));
    float t = v * (1.0f - (s * (1.0f - ff)));

    switch (i) {
        case 0:
            r = v;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = v;
            b = p;
            break;
        case 2:
            r = p;
            g = v;
            b = t;
            break;
        case 3:
            r = p;
            g = q;
            b = v;
            break;
        case 4:
            r = t;
            g = p;
            b = v;
            break;
        case 5:
        default:
            r = v;
            g = p;
            b = q;
            break;
    }
}

#endif

