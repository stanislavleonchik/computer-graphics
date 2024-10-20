#include "Buttons.h"

unsigned char* applyGrayscaleFormula1(unsigned char* data, int width, int height, int channels)
{
    unsigned char* data_g = data;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * channels;
            unsigned char r = data[index];
            unsigned char g = data[index + 1];
            unsigned char b = data[index + 2];

            // Вычисляем яркость по формуле Y = 0.3R + 0.59G + 0.11B
            unsigned char gray = static_cast<unsigned char>(0.3f * r + 0.59f * g + 0.11f * b);

            // Присваиваем пикселю значение серого
            data_g[index] = gray;
            data_g[index + 1] = gray;
            data_g[index + 2] = gray;
        }
    }
    return data_g;
}

unsigned char* applyGrayscaleFormula2(unsigned char* data, int width, int height, int channels)
{
    unsigned char* data_g = data;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * channels;
            unsigned char r = data[index];
            unsigned char g = data[index + 1];
            unsigned char b = data[index + 2];

            unsigned char gray = static_cast<unsigned char>(0.21f * r + 0.72f * g + 0.07f * b);

            // Присваиваем пикселю значение серого
            data_g[index] = gray;
            data_g[index + 1] = gray;
            data_g[index + 2] = gray;
        }
    }
    return data_g;
}

unsigned char* applyGrayDifference(unsigned char* data1, unsigned char* data2, int width, int height, int channels)
{
    unsigned char* data_g = data1;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            int index = (y * width + x) * channels;
            unsigned char g1 = data1[index];
            unsigned char g2 = data2[index];

            unsigned char gray = g1-g2;

            // Присваиваем пикселю значение серого
            data_g[index] = gray;
            data_g[index + 1] = gray;
            data_g[index + 2] = gray;
        }
    }
    return data_g;
}
