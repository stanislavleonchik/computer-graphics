#ifndef COMPUTER_GRAPHICS_WU_H
#define COMPUTER_GRAPHICS_WU_H

#include "../includes.h"

static inline float fpart(float x) {
    return x - floorf(x);
}

static inline float rfpart(float x) {
    return 1.0f - fpart(x);
}

static inline int ipart(float x) {
    return (int)floorf(x);
}

void setPixel(unsigned char* image, int width, int height, int channels, int x, int y, float brightness) {
    if (x < 0 || x >= width || y < 0 || y >= height) return;

    int index = (y * width + x) * channels;

    for (int c = 0; c < 3; ++c) {
        image[index + c] = (unsigned char)(image[index + c] * (1.0f - brightness));
    }

    if (channels == 4) {
        image[index + 3] = 255;
    }
}

void draw_wu_line(unsigned char* image, int width, int height, int channels,
                  int x0, int y0, int x1, int y1) {
    int steep = abs(y1 - y0) > abs(x1 - x0);

    if (steep) {
        int temp;
        temp = x0; x0 = y0; y0 = temp;
        temp = x1; x1 = y1; y1 = temp;
    }

    if (x0 > x1) {
        int temp;
        temp = x0; x0 = x1; x1 = temp;
        temp = y0; y0 = y1; y1 = temp;
    }

    float dx = x1 - x0;
    float dy = y1 - y0;
    float gradient = dx == 0.0f ? 1.0f : dy / dx;

    float xend = roundf(x0);
    float yend = y0 + gradient * (xend - x0);
    float xgap = rfpart(x0 + 0.5f);
    int xpxl1 = (int)xend;
    int ypxl1 = ipart(yend);

    if (steep) {
        setPixel(image, width, height, channels, ypxl1, xpxl1, rfpart(yend) * xgap);
        setPixel(image, width, height, channels, ypxl1 + 1, xpxl1, fpart(yend) * xgap);
    } else {
        setPixel(image, width, height, channels, xpxl1, ypxl1, rfpart(yend) * xgap);
        setPixel(image, width, height, channels, xpxl1, ypxl1 + 1, fpart(yend) * xgap);
    }

    float intery = yend + gradient;

    xend = roundf(x1);
    yend = y1 + gradient * (xend - x1);
    xgap = fpart(x1 + 0.5f);
    int xpxl2 = (int)xend;
    int ypxl2 = ipart(yend);

    if (steep) {
        setPixel(image, width, height, channels, ypxl2, xpxl2, rfpart(yend) * xgap);
        setPixel(image, width, height, channels, ypxl2 + 1, xpxl2, fpart(yend) * xgap);
    } else {
        setPixel(image, width, height, channels, xpxl2, ypxl2, rfpart(yend) * xgap);
        setPixel(image, width, height, channels, xpxl2, ypxl2 + 1, fpart(yend) * xgap);
    }

    if (steep) {
        for (int x = xpxl1 + 1; x < xpxl2; x++) {
            setPixel(image, width, height, channels, ipart(intery), x, rfpart(intery));
            setPixel(image, width, height, channels, ipart(intery) + 1, x, fpart(intery));
            intery += gradient;
        }
    } else {
        for (int x = xpxl1 + 1; x < xpxl2; x++) {
            setPixel(image, width, height, channels, x, ipart(intery), rfpart(intery));
            setPixel(image, width, height, channels, x, ipart(intery) + 1, fpart(intery));
            intery += gradient;
        }
    }
}
#endif
