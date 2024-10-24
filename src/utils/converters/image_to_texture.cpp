
#include "GLFW/glfw3.h"

export module image_to_texture;

export GLuint image_to_texture(
        const unsigned char* image,
        const int width,
        const int height,
        const int channels
        ) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, (int)format, width, height, 0, format, GL_UNSIGNED_BYTE, image);


    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}
