#ifndef COMPUTER_GRAPHICS_DRAW_BRESENHAM_LINE_H
#define COMPUTER_GRAPHICS_DRAW_BRESENHAM_LINE_H

#include "../includes.h"

void draw_bresenham_line(unsigned char* image, int width, int height, int channels, int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        if (x0 >= 0 && x0 < width && y0 >= 0 && y0 < height) {
            int index = (y0 * width + x0) * channels;
            image[index] = 0;
            image[index + 1] = 0;
            image[index + 2] = 0;
            if (channels == 4) {
                image[index + 3] = 255;
            }
        }

        if (x0 == x1 && y0 == y1) break;
        int e2 = err * 2;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

#endif
