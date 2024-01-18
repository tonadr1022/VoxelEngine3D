//
// Created by Tony Adriansen on 1/18/24.
//

#include <fstream>
#include "JsonUtils.hpp"


nlohmann::json JsonUtils::openJson(const std::string &path) {

  std::ifstream jsonFile(path);
  if (!jsonFile.is_open()) {
    throw std::runtime_error(
        "Failed to open file: " + path + "\n");
  }
  nlohmann::json data;
  jsonFile >> data;
  jsonFile.close();
  return data;
}