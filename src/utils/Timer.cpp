//
// Created by Tony Adriansen on 12/17/23.
//

#include "Timer.hpp"

Timer::Timer(const std::string &message) : m_message(message) {
    m_startTime = std::chrono::high_resolution_clock::now();
}


Timer::~Timer() {
    auto m_endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(m_endTime - m_startTime);
    std::cout << m_message << " took: " << duration.count() << " ms" << std::endl;
}
