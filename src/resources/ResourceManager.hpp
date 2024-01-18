//
// Created by Tony Adriansen on 11/17/23.
//

#ifndef VOXEL_ENGINE_RESOURCEMANAGER_HPP
#define VOXEL_ENGINE_RESOURCEMANAGER_HPP

#include "Image.hpp"
#include "../EngineConfig.hpp"

const std::string BLOCK_DIR_PATH = "resources/textures/block";
const std::filesystem::path BLOCK_BREAK_PATH = "resources/textures/block_break.png";

class ResourceManager {
 public:
  static bool texturesLoaded;
  static void makeTexture(const std::string &texturePath,
                          const std::string &textureName,
                          bool flipVertically = false);
  static void makeTexture2dArray(std::vector<Image> &images,
                                 const std::string &textureName,int width, int height);
  static unsigned int getTexture(const std::string &textureName);
  static void loadTextures();
  static Image loadImage(const std::filesystem::path &imagePath,
                         bool flipVertically = false);



  static std::unordered_map<std::string, int> filenameToTexIndex;

 private:
  static std::unordered_map<std::string, unsigned int> textures;
  static std::vector<Image> loadBlockImages();
  static std::vector<Image> loadBlockBreakImages();
};

#endif //VOXEL_ENGINE_RESOURCEMANAGER_HPP
