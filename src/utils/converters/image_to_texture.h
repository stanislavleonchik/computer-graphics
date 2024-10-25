#include <GLFW/glfw3.h>

GLuint image_to_texture(
        const unsigned char* image,
        const int width,
        const int height,
        const int channels
        ) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, (int)format, width, height, 0, format, GL_UNSIGNED_BYTE, image);


    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}
