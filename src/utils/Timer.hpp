//
// Created by Tony Adriansen on 12/17/23.
//

#ifndef VOXEL_ENGINE_TIMER_HPP
#define VOXEL_ENGINE_TIMER_HPP

#include "../EngineConfig.hpp"

class Timer {
public:
    Timer(const std::string& message = "");
    ~Timer();


private:
    const std::string& m_message;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
};


#endif //VOXEL_ENGINE_TIMER_HPP
