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
        unsigned char original = image[index + c];
        unsigned char value = (unsigned char)(original * (1.0f - brightness));
        image[index + c] = value;
    }

    if (channels == 4) {
        image[index + 3] = 255;
    }
}

void plot_thick_pixel(unsigned char* image, int width, int height, int channels,
                      int x, int y, float brightness, float cur_thickness, int steep) {
    int half_cur_thickness = (int)(cur_thickness / 2);
    for (int t = -half_cur_thickness; t <= half_cur_thickness; ++t) {
        int nx = x + (steep ? t : 0);
        int ny = y + (steep ? 0 : t);
        setPixel(image, width, height, channels, nx, ny, brightness);
    }
}

void draw_wu_line(unsigned char* image, int width, int height, int channels,
                  int x0, int y0, int x1, int y1, int cur_thickness) {
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

    float intery = yend + gradient;

    xend = roundf(x1);
    yend = y1 + gradient * (xend - x1);
    xgap = fpart(x1 + 0.5f);
    int xpxl2 = (int)xend;
    int ypxl2 = ipart(yend);

    if (steep) {
        plot_thick_pixel(image, width, height, channels, ypxl1, xpxl1, rfpart(yend) * xgap, cur_thickness, steep);
        plot_thick_pixel(image, width, height, channels, ypxl1 + 1, xpxl1, fpart(yend) * xgap, cur_thickness, steep);

        plot_thick_pixel(image, width, height, channels, ypxl2, xpxl2, rfpart(yend) * xgap, cur_thickness, steep);
        plot_thick_pixel(image, width, height, channels, ypxl2 + 1, xpxl2, fpart(yend) * xgap, cur_thickness, steep);
    } else {
        plot_thick_pixel(image, width, height, channels, xpxl1, ypxl1, rfpart(yend) * xgap, cur_thickness, steep);
        plot_thick_pixel(image, width, height, channels, xpxl1, ypxl1 + 1, fpart(yend) * xgap, cur_thickness, steep);

        plot_thick_pixel(image, width, height, channels, xpxl2, ypxl2, rfpart(yend) * xgap, cur_thickness, steep);
        plot_thick_pixel(image, width, height, channels, xpxl2, ypxl2 + 1, fpart(yend) * xgap, cur_thickness, steep);
    }

    if (steep) {
        for (int x = xpxl1 + 1; x < xpxl2; x++) {
            float brightness1 = rfpart(fpart(intery));
            float brightness2 = fpart(fpart(intery));

            plot_thick_pixel(image, width, height, channels, ipart(intery), x, brightness1, cur_thickness, steep);
            plot_thick_pixel(image, width, height, channels, ipart(intery) + 1, x, brightness2, cur_thickness, steep);

            intery += gradient;
        }
    } else {
        for (int x = xpxl1 + 1; x < xpxl2; x++) {
            float brightness1 = rfpart(fpart(intery));
            float brightness2 = fpart(fpart(intery));

            plot_thick_pixel(image, width, height, channels, x, ipart(intery), brightness1, cur_thickness, steep);
            plot_thick_pixel(image, width, height, channels, x, ipart(intery) + 1, brightness2, cur_thickness, steep);

            intery += gradient;
        }
    }
}
#endif
