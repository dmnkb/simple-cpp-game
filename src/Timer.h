#pragma once

#include "pch.h"
#include <chrono>

class Timer
{
  public:
    using Clock = std::chrono::high_resolution_clock;

    void start()
    {
        m_StartTime = Clock::now();
        m_bRunning = true;
    }

    void stop()
    {
        m_EndTime = Clock::now();
        m_bRunning = false;
    }

    double elapsedMilliseconds()
    {
        auto endTime = m_bRunning ? Clock::now() : m_EndTime;

        return std::chrono::duration_cast<std::chrono::microseconds>(endTime - m_StartTime).count() / 1000.0;
    }

    double elapsedSeconds()
    {
        return elapsedMilliseconds() / 1000.0;
    }

  private:
    std::chrono::time_point<Clock> m_StartTime;
    std::chrono::time_point<Clock> m_EndTime;
    bool m_bRunning = false;
};
