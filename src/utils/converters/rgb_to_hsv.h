#include <algorithm>

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
