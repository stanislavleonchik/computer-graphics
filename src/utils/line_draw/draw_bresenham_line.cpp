
export module draw_bresenham_line;

import <cmath>;

export void draw_bresenham_line(unsigned char* image, int width, int height, int channels, int x0, int y0, int x1, int y1, int thickness = 1) {
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        for (int tx = -thickness / 2; tx <= thickness / 2; ++tx)
            for (int ty = -thickness / 2; ty <= thickness / 2; ++ty) {
                int nx = x0 + tx;
                int ny = y0 + ty;

                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    int index = (ny * width + nx) * channels;
                    image[index] = 0;
                    image[index + 1] = 0;
                    image[index + 2] = 0;
                    if (channels == 4) {
                        image[index + 3] = 255;
                    }
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
