//
// Created by Tony Adriansen on 12/22/23.
//

#include "WorldSave.hpp"
#include "../../EngineConfig.hpp"

WorldSave::~WorldSave() = default;

WorldSave::WorldSave(const std::string &path) : m_path(path) {
//    std::ifstream file(path, std::ios::in | std::ios::binary);
//    if (!file) {
//        std::ofstream createFile(path, std::ios::in | std::ios::binary);
//        if (!createFile) {
//            std::cerr << "Failed to create world save file" << std::endl;
//            exit(EXIT_FAILURE);
//        } else {
//            createFile.close();
//        }
//    } else {
//
//
//        file.close();
//    }
}

void WorldSave::saveData() {
    // save chunks into file
}

void WorldSave::commitChunkData(const glm::ivec2 &pos, Block *data) {
    m_chunkData[pos] = std::copy(data, data + CHUNK_VOLUME, new Block[CHUNK_VOLUME]);
}
