//
// Created by Tony Adriansen on 12/17/23.
//

#ifndef VOXEL_ENGINE_IGAMESTATE_HPP
#define VOXEL_ENGINE_IGAMESTATE_HPP


#include "../core/ISingleton.hpp"

class IGameState : public ISingleton {
public:
    virtual void update() = 0;
    virtual void render() = 0;

    virtual IGameState instance() = 0;

private:

protected:
    IGameState() {}

};


#endif //VOXEL_ENGINE_IGAMESTATE_HPP
