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

void setPixel(uint8_t* image, int width, int height, int channels, int x, int y, float brightness) {
    if (x < 0 || x >= width || y < 0 || y >= height) return;

    int index = (y * width + x) * channels;

    for (int c = 0; c < 3; ++c) {
        uint8_t original = image[index + c];
        uint8_t value = (uint8_t)(original * (1.0f - brightness));
        image[index + c] = value;
    }

    if (channels == 4) {
        image[index + 3] = 255;
    }
}

void plot_thick_pixel(uint8_t* image, int width, int height, int channels, int x, int y, float brightness, float cur_thickness, int steep) {
    int half_cur_thickness = (int)(cur_thickness / 2);
    for (int t = -half_cur_thickness; t <= half_cur_thickness; ++t) {
        int nx = x + (steep ? t : 0);
        int ny = y + (steep ? 0 : t);
        setPixel(image, width, height, channels, nx, ny, brightness);
    }
}

void draw_wu_line(uint8_t* image, int width, int height, int channels, int x0, int y0, int x1, int y1, int cur_thickness) {
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

void setPixelAlpha(uint8_t* image, int width, int height, int channels, int x, int y, uint8_t r, uint8_t g, uint8_t b, float alpha) {
    if (x < 0 || x >= width || y < 0 || y >= height) return;

    int index = (y * width + x) * channels;

    image[index] = static_cast<uint8_t>(r * alpha + image[index] * (1 - alpha));
    image[index + 1] = static_cast<uint8_t>(g * alpha + image[index + 1] * (1 - alpha));
    image[index + 2] = static_cast<uint8_t>(b * alpha + image[index + 2] * (1 - alpha));

    if (channels == 4) {
        image[index + 3] = 255;
    }
}

void draw_line_basic(uint8_t* image, int width, int height, int channels, int x0, int y0, int x1, int y1, uint8_t r, uint8_t g, uint8_t b) {
    int dx = abs(x1 - x0);
    int dy = -abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;

    while (true) {
        setPixelAlpha(image, width, height, channels, x0, y0, r, g, b, 1.0f);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void draw_line_gupta_sproull(uint8_t* image, int width, int height, int channels,
                             int x0, int y0, int x1, int y1, int thickness,
                             uint8_t r, uint8_t g, uint8_t b) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int adx = abs(dx);
    int ady = abs(dy);

    int xDir = (dx >= 0) ? 1 : -1;
    int yDir = (dy >= 0) ? 1 : -1;

    if (adx == 0 && ady == 0) {
        // Line is a single point
        for (int t = -thickness / 2; t <= thickness / 2; ++t) {
            setPixelAlpha(image, width, height, channels, x0 + t, y0, r, g, b, 1.0f);
            setPixelAlpha(image, width, height, channels, x0, y0 + t, r, g, b, 1.0f);
        }
        return;
    }

    float length = sqrt(dx * dx + dy * dy);
    float ux = dx / length;
    float uy = dy / length;

    for (int t = -thickness / 2; t <= thickness / 2; ++t) {
        int xShift = -uy * t;
        int yShift = ux * t;

        int xStart = x0 + xShift;
        int yStart = y0 + yShift;
        int xEnd = x1 + xShift;
        int yEnd = y1 + yShift;

        draw_line_basic(image, width, height, channels, xStart, yStart, xEnd, yEnd, r, g, b);
    }
}

#endif
