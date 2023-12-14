//
// Created by Tony Adriansen on 11/17/23.
//

#define STB_IMAGE_IMPLEMENTATION
#include "ResourceManager.hpp"
#include <glad/glad.h>
#include "../vendor/stb_image/stb_image.h"

std::unordered_map<std::string, unsigned int> ResourceManager::textures;

void ResourceManager::makeTexture(const std::string &texturePath, const std::string &textureName,
                                  bool flipVertically) {
    int width, height, nrChannels;
    if (!flipVertically)
        stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels,
                                    STBI_rgb_alpha);
    if (!data) {
        throw std::runtime_error("Failed to load texture: " + texturePath);
    }
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    textures.emplace(textureName, texture);

    // configure sampler
    // s = x-axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // t = y-axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);
}

unsigned int ResourceManager::getTexture(const std::string &textureName) {
    auto it = textures.find(textureName);
    if (it != textures.end()) {
        return it->second;
    }
    throw std::runtime_error("Texture not found: " + textureName);

}



void
ResourceManager::makeTexture2dArray(const std::string &texturePath, const std::string &textureName,
                                    bool flipVertically) {
    const uint8_t tileWidth = 32;
    const uint8_t tileHeight = 32;
    const uint8_t tilesPerRow = 16;
    const uint8_t tilesPerColumn = 16;

    int width, height, nrChannels;
    if (!flipVertically)
        stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels,
                                    STBI_rgb_alpha);
    if (!data) {
        throw std::runtime_error("Failed to load texture: " + texturePath);
    }
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);


    stbi_image_free(data);
    textures.emplace(textureName, texture);

    // configure sampler
    // s = x-axis
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    // t = y-axis
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, tileWidth, tileHeight, tilesPerColumn * tilesPerRow, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 nullptr);
    for (int tileY = 0; tileY < tilesPerColumn; tileY++) {
        for (int tileX = 0; tileX < tilesPerRow; tileX++) {
            int tileIndex = tileY * tilesPerRow + tileX;
            int x = tileX * tileWidth;
            int y = tileY * tileHeight;
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, tileIndex, tileWidth, tileHeight, 1, GL_RGBA, GL_UNSIGNED_BYTE, data + (x + y * width) * 4);
        }
    }
//    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
}