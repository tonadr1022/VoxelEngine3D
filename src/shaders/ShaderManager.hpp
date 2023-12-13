//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_SHADERMANAGER_HPP
#define VOXEL_ENGINE_SHADERMANAGER_HPP

#include "Shader.hpp"

class ShaderManager {
public:

    static void addShader(std::shared_ptr<Shader> shader, const std::string &name);

    static void useShader(const std::string &name);

    static std::shared_ptr<Shader> getShader(const std::string &name);

private:
    static std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;


};


#endif //VOXEL_ENGINE_SHADERMANAGER_HPP
