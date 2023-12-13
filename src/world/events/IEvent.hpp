//
// Created by Tony Adriansen on 11/29/23.
//

#ifndef VOXEL_ENGINE_IEVENT_HPP
#define VOXEL_ENGINE_IEVENT_HPP

class IEvent {
public:
    virtual void handle() = 0;
};

#endif //VOXEL_ENGINE_IEVENT_HPP

