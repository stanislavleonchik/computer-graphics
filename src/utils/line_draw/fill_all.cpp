
export module fill_all;

import Color;

export void fill_all(unsigned char* image, int width, int height, int channels, Color color = {30, 31, 33}) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = (y * width + x) * channels;  // Индекс пикселя в массиве изображения
            image[index] = color.r;      // Красный канал
            image[index + 1] = color.g;  // Зеленый канал
            image[index + 2] = color.b;  // Синий канал
            if (channels == 4) {
                image[index + 3] = 255;  // Альфа-канал, если он есть
            }
        }
    }
}
