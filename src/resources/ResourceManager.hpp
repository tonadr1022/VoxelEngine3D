//
// Created by Tony Adriansen on 11/17/23.
//

#ifndef VOXEL_ENGINE_RESOURCEMANAGER_HPP
#define VOXEL_ENGINE_RESOURCEMANAGER_HPP

#include "Image.hpp"
#include "../EngineConfig.hpp"

class ResourceManager {
 public:
  static bool texturesLoaded;
  static void makeTexture(const std::string& texturePath,
                          const std::string& textureName,
                          bool flipVertically = false);
  static void makeTexture2dArray(std::vector<Image>& images,
                                 const std::string& textureName, int width, int height);
  static unsigned int getTexture(const std::string& textureName);
  static void loadTextures();
  static Image loadImage(const std::filesystem::path& imagePath,
                         bool flipVertically = false);
  static std::unordered_map<std::string, int> filenameToTexIndex;
  static std::unordered_map<std::string, glm::lowp_ivec3> filenameToAvgColor;

 private:
  static std::unordered_map<std::string, unsigned int> textures;
  static void loadBlockImages(std::vector<Image>& result);
  static void loadBlockBreakImages(std::vector<Image>& result);
};

#endif //VOXEL_ENGINE_RESOURCEMANAGER_HPP
