//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_SHADER_HPP
#define VOXEL_ENGINE_SHADER_HPP

#include "../EngineConfig.hpp"

class Shader {
 public:
  Shader(const std::string &vertexPath,
         const std::string &fragmentPath,
         const std::string &geometryPath = "");
  ~Shader();

  void use() const;
  void unbind() const;
  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, int value) const;
  void setFloat(const std::string &name, float value) const;
  void setVec3(const std::string &name, const glm::vec3 &vector) const;
  void setIVec2(const std::string &name, const glm::ivec2 &vector) const;
  void setMat4(const std::string &name, const glm::mat4 &matrix) const;
  GLint getUniformLocation(const std::string &name) const;

 private:
  static GLint makeModule(const std::string &filepath, GLint module_type);

  GLint shaderProgram;
  mutable std::unordered_map<std::string, GLint> uniformLocationCache;

};

#endif //VOXEL_ENGINE_SHADER_HPP
