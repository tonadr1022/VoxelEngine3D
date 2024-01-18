//
// Created by Tony Adriansen on 1/18/24.
//

#ifndef VOXEL_ENGINE_SRC_UTILS_JSONUTILS_HPP_
#define VOXEL_ENGINE_SRC_UTILS_JSONUTILS_HPP_

#include "json/json.hpp"

class JsonUtils {
 public:
  static nlohmann::json openJson(const std::string &path);
};

#endif //VOXEL_ENGINE_SRC_UTILS_JSONUTILS_HPP_
