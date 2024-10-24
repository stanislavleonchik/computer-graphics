
export module rgb_to_hsv_image;

import <vector>;
import rgb_to_hsv;

using std::vector;

export vector<float> rgv_to_hsv_image_transform(
        const unsigned char* image,
        const int width,
        const int height,
        const int channels
        ) {
    vector<float> hsv_image(width * height * 3);
    for (int i = 0; i < width * height; ++i) {
        float r = (float)image[i * channels] / 255.0f;
        float g = (float)image[i * channels + 1] / 255.0f;
        float b = (float)image[i * channels + 2] / 255.0f;
        float h, s, v;
        rgb_to_hsv(r, g, b, h, s, v);
        hsv_image[i * 3] = h;
        hsv_image[i * 3 + 1] = s;
        hsv_image[i * 3 + 2] = v;
    }

    return hsv_image;
}
