//
// Created by Tony Adriansen on 11/17/23.
//

#define STB_IMAGE_IMPLEMENTATION
#include "ResourceManager.hpp"
#include "Image.hpp"

std::unordered_map<std::string, unsigned int> ResourceManager::textures;

void ResourceManager::makeTexture(const std::string &texturePath,
                                  const std::string &textureName,
                                  bool flipVertically) {
  Image image = loadImage(texturePath, flipVertically);
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGBA,
               image.width,
               image.height,
               0,
               GL_RGBA,
               GL_UNSIGNED_BYTE,
               image.pixels.data());
  stbi_image_free(image.pixels.data());
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
ResourceManager::makeTexture2dArray(const std::string &texturePath,
                                    const std::string &textureName,
                                    bool flipVertically) {
  Image image = loadImage(texturePath, flipVertically);
  constexpr int tilesPerRow = 16;
  constexpr int tilesPerColumn = 16;
  const GLsizei tileWidth = image.width / tilesPerRow;
  const GLsizei tileHeight = image.height / tilesPerColumn;

  uint32_t texture;
  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

  glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, tileWidth, tileHeight,
               tilesPerColumn * tilesPerRow, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               image.pixels.data());
  textures.emplace(textureName, texture);

  // configure sampler
  // s = x-axis
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  // t = y-axis
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D_ARRAY,
                  GL_TEXTURE_MIN_FILTER,
                  GL_NEAREST_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  std::vector<Image> subImages;

  // create sub images on CPU
  for (int tileX = 0; tileX < tilesPerColumn; tileX++) {
    for (int tileY = 0; tileY < tilesPerRow; tileY++) {
      uint32_t offsetX = tileX * tileWidth;
      uint32_t offsetY = tileY * tileHeight;
      subImages.emplace_back(image.subImage({offsetX, offsetY},
                                            {tileWidth, tileHeight}));
    }
  }

  // create sub images on GPU
  for (int subImageIndex = 0; subImageIndex < subImages.size();
       subImageIndex++) {
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
                    0,
                    0,
                    0,
                    subImageIndex,
                    static_cast<GLsizei>(tileWidth),
                    static_cast<GLsizei>(tileHeight),
                    1,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    subImages[subImageIndex].pixels.data());
  }
  glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
  glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

Image ResourceManager::loadImage(const std::string &imagePath,
                                 bool flipVertically) {
  int width, height, nrChannels;
  if (!flipVertically)
    stbi_set_flip_vertically_on_load(true);
  unsigned char
      *data = stbi_load(imagePath.c_str(), &width, &height, &nrChannels,
                        STBI_rgb_alpha);
  if (!data) {
    throw std::runtime_error("Failed to load texture: " + imagePath);
  }

  Image image
      {width, height, std::vector<uint8_t>(data, data + width * height * 4)};
  stbi_image_free(data);
  return image;
}