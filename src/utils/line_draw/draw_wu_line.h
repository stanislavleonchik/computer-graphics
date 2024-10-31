#pragma once

#include <cmath>
#include <algorithm>

static inline float fpart(float x) { return x - std::floor(x); }
static inline float rfpart(float x) { return 1.0f - fpart(x); }
static inline int ipart(float x) { return static_cast<int>(std::floor(x)); }

inline void setPixel(unsigned char* image, int width, int height, int channels, int x, int y, float brightness) {
    if (x < 0 || x >= width || y < 0 || y >= height) return;
    int index = (y * width + x) * channels;
    for (int c = 0; c < 3; ++c)
        image[index + c] = static_cast<unsigned char>(image[index + c] * (1.0f - brightness));
    if (channels == 4) {
        image[index + 3] = 255;
    }
}

inline void plotLineSegment(unsigned char* image, int width, int height, int channels, int x, int y, float brightness, bool steep) {
    if (steep) setPixel(image, width, height, channels, y, x, brightness);
    else setPixel(image, width, height, channels, x, y, brightness);
}

inline void draw_wu_line(unsigned char* image, int width, int height, int channels, int x0, int y0, int x1, int y1) {
    bool steep = std::abs(y1 - y0) > std::abs(x1 - x0);
    if (steep) std::swap(x0, y0), std::swap(x1, y1);
    if (x0 > x1) std::swap(x0, x1), std::swap(y0, y1);
    float dx = x1 - x0, dy = y1 - y0;
    float gradient = (dx == 0.0f) ? 1.0f : dy / dx;

    float xEnd = std::round(x0);
    float yEnd = y0 + gradient * (xEnd - x0);
    float xGap = rfpart(x0 + 0.5f);
    int xPixel = static_cast<int>(xEnd);
    int yPixel = ipart(yEnd);
    plotLineSegment(image, width, height, channels, xPixel, yPixel, rfpart(yEnd) * xGap, steep);
    plotLineSegment(image, width, height, channels, xPixel, yPixel + 1, fpart(yEnd) * xGap, steep);
    float intery = yEnd + gradient;

    xEnd = std::round(x1);
    yEnd = y1 + gradient * (xEnd - x1);
    xGap = fpart(x1 + 0.5f);
    xPixel = static_cast<int>(xEnd);
    yPixel = ipart(yEnd);
    plotLineSegment(image, width, height, channels, xPixel, yPixel, rfpart(yEnd) * xGap, steep);
    plotLineSegment(image, width, height, channels, xPixel, yPixel + 1, fpart(yEnd) * xGap, steep);

    for (int x = static_cast<int>(x0) + 1; x < static_cast<int>(xEnd); ++x) {
        plotLineSegment(image, width, height, channels, x, ipart(intery), rfpart(intery), steep);
        plotLineSegment(image, width, height, channels, x, ipart(intery) + 1, fpart(intery), steep);
        intery += gradient;
    }
}