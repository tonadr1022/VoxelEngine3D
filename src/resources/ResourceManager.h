//
// Created by Tony Adriansen on 11/17/23.
//

#ifndef VOXEL_ENGINE_RESOURCEMANAGER_H
#define VOXEL_ENGINE_RESOURCEMANAGER_H

#include <string>

class ResourceManager {
public:
    static void makeTexture(const std::string& texturePath, const std::string& textureName, bool flipVertically = false);
    static unsigned int getTexture(const std::string &textureName);

private:
    static std::unordered_map<std::string, unsigned int> textures;

};

#endif //VOXEL_ENGINE_RESOURCEMANAGER_H
