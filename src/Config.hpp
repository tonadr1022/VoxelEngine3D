//
// Created by Tony Adriansen on 12/11/23.
//

#ifndef VOXEL_ENGINE_CONFIG_HPP
#define VOXEL_ENGINE_CONFIG_HPP

class Config {
 public:
  static bool getUseAmbientOcclusion();
  static void setUseAmbientOcclusion(bool pUseAmbientOcclusion);

 private:
  static bool useAmbientOcclusion;
};

#endif //VOXEL_ENGINE_CONFIG_HPP
