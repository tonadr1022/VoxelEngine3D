//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_SHADER_H
#define VOXEL_ENGINE_SHADER_H

#include <glad/glad.h>
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>


class Shader {
public:
    Shader(const std::string &vertexPath, const std::string &fragmentPath, const std::string &geometryPath = "");

    void use() const;

    void unbind() const;

    void setBool(const std::string &name, bool value) const;

    void setInt(const std::string &name, int value) const;

    void setFloat(const std::string &name, float value) const;

    void setVec3(const std::string &name, glm::vec3 &vector) const;

    void setMat4(const std::string &name, glm::mat4 &matrix) const;

    GLint getUniformLocation(const std::string &name) const;

private:
    GLint shaderProgram;

    mutable std::unordered_map<std::string, GLint> uniformLocationCache;
    static GLint makeModule(const std::string &filepath, GLint module_type);
};

#endif //VOXEL_ENGINE_SHADER_H
