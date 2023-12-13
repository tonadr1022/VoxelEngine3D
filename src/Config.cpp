//
// Created by Tony Adriansen on 12/11/23.
//

#include "Config.hpp"

bool Config::getUseAmbientOcclusion() {
    return useAmbientOcclusion;
}

void Config::setUseAmbientOcclusion(bool pUseAmbientOcclusion) {
    Config::useAmbientOcclusion = pUseAmbientOcclusion;
}

bool Config::useAmbientOcclusion = true;
