
#include "GLFW/glfw3.h"

export module update_texture;

export void update_texture(
        GLuint textureID,
        const unsigned char* image,
        const int width,
        const int height,
        const int channels
        ) {
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, (int)format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
    glBindTexture(GL_TEXTURE_2D, 0);
}
