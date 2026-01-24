#define STB_IMAGE_IMPLEMENTATION

#include "TextureLoader.hpp"
#include <../external/images/stb_image.h>
#include <iostream>

unsigned int loadTextureFromFile(const char* filename) {
    // Variables to store image dimensions and channel count
    int width, height, channels;

    // Load image data using stb_image
    // Force 4 channels (RGBA) even if original image has fewer
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 4);
    if (!data) {
        // Print error if loading fails and return 0 as invalid texture
        std::cerr << "Failed to load texture: " << filename << std::endl;
        return 0;
    }

    // Generate a new OpenGL texture ID
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Upload image data to the GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Unbind the texture and free image memory
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

    // Return the generated texture ID
    return textureID;
}
