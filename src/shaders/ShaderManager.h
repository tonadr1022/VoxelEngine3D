//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_SHADERMANAGER_H
#define VOXEL_ENGINE_SHADERMANAGER_H

#include "Shader.h"

class ShaderManager {
public:

    static void addShader(std::shared_ptr<Shader> shader, const std::string &name);

    static void useShader(const std::string &name);

    static std::shared_ptr<Shader> getShader(const std::string &name);

private:
    static std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;


};


#endif //VOXEL_ENGINE_SHADERMANAGER_H
