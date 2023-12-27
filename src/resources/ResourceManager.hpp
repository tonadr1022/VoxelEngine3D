//
// Created by Tony Adriansen on 11/17/23.
//

#ifndef VOXEL_ENGINE_RESOURCEMANAGER_HPP
#define VOXEL_ENGINE_RESOURCEMANAGER_HPP

#include "Image.hpp"
#include "../EngineConfig.hpp"

class ResourceManager {
 public:
  static void makeTexture(const std::string &texturePath,
                          const std::string &textureName,
                          bool flipVertically = false);
  static void makeTexture2dArray(const std::string &texturePath,
                                 const std::string &textureName,
                                 bool flipVertically = false);
  static unsigned int getTexture(const std::string &textureName);
  static Image loadImage(const std::string &imagePath,
                         bool flipVertically = false);

 private:
  static std::unordered_map<std::string, unsigned int> textures;

};

#endif //VOXEL_ENGINE_RESOURCEMANAGER_HPP
