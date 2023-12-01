//
// Created by Tony Adriansen on 11/29/23.
//

#ifndef VOXEL_ENGINE_IEVENT_H
#define VOXEL_ENGINE_IEVENT_H

class IEvent {
public:
    virtual void handle() = 0;
};

#endif //VOXEL_ENGINE_IEVENT_H

