#include "GLFW/glfw3.h"

void update_texture(
        GLuint textureID,
        const unsigned char* image,
        const int width,
        const int height,
        const int channels
        ) {
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLenum format = GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, (int)format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
    glBindTexture(GL_TEXTURE_2D, 0);
}
