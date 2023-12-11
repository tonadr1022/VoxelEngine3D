//
// Created by Tony Adriansen on 12/11/23.
//

#ifndef VOXEL_ENGINE_CONFIG_H
#define VOXEL_ENGINE_CONFIG_H


class Config {
public:
    static bool getUseAmbientOcclusion();

    static void setUseAmbientOcclusion(bool pUseAmbientOcclusion);

private:
    static bool useAmbientOcclusion;
};


#endif //VOXEL_ENGINE_CONFIG_H
