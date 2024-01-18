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

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels.data());
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
  if (!ResourceManager::texturesLoaded) ResourceManager::loadTextures();
  auto it = textures.find(textureName);
  if (it != textures.end()) {
    return it->second;
  }
  throw std::runtime_error("Texture not found: " + textureName);
}

std::unordered_map<std::string, int> ResourceManager::filenameToTexIndex;

void
ResourceManager::makeTexture2dArray(const std::string &texturePath,
                                    const std::string &textureName,
                                    bool flipVertically) {
  int TEX_WIDTH = 16;
  int TEX_HEIGHT = 16;
  std::vector<Image> images;

  int texIndex = 0;
  for (const auto &entry : std::filesystem::directory_iterator(BLOCK_DIR_PATH)) {
    if (std::filesystem::is_regular_file(entry.path()) && entry.path().extension() == ".png") {
      Image image = loadImage(entry.path(), true);
      filenameToTexIndex[entry.path().stem().string()] = texIndex;
      images.push_back(image);
      texIndex++;
    }
  }

  uint32_t texture;
  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

  glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, TEX_WIDTH, TEX_HEIGHT,
               images.size(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
               nullptr);
  textures.emplace(textureName, texture);

  // configure sampler
  // s = x-axis
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  // t = y-axis
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // create sub images on GPU
  for (int index = 0; index < images.size(); index++) {
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, index, static_cast<GLsizei>(TEX_WIDTH),
                    static_cast<GLsizei>(TEX_HEIGHT), 1, GL_RGBA, GL_UNSIGNED_BYTE,
                    images[index].pixels.data());
  }

  glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
  glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

Image ResourceManager::loadImage(const std::string &imagePath,
                                 bool flipVertically) {
  int width, height, nrChannels;
  if (!flipVertically)
    stbi_set_flip_vertically_on_load(true);
  unsigned char *data = stbi_load(imagePath.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
  if (!data) {
    throw std::runtime_error("Failed to load texture: " + imagePath);
  }

  Image image{width, height, std::vector<uint8_t>(data, data + width * height * 4)};
  stbi_image_free(data);
  return image;
}

void ResourceManager::loadTextures() {
  if (texturesLoaded) return;
  makeTexture2dArray("resources/textures/default_pack_512.png", "texture_atlas", true);
  texturesLoaded = true;
}

bool ResourceManager::texturesLoaded = false;
