//
// Created by Tony Adriansen on 12/17/23.
//

#include "Timer.hpp"

Timer::Timer(const std::string &message) : m_message(message) {
    m_startTime = std::chrono::high_resolution_clock::now();
}


Timer::~Timer() {
    auto m_endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(m_endTime - m_startTime);
    std::cout << m_message << " took: " << static_cast<float>(duration.count()) / 1000.0f << " ms" << std::endl;
}
