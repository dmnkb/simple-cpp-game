#pragma once

#include "core/Core.h"
#include "core/GPUTimer.h"
#include "core/Timer.h"

namespace Engine
{

using FrameTimePerRegion = std::vector<std::pair<std::string, double>>;
using GPUTimePerRegion = std::vector<std::pair<std::string, double>>;
using DrawCallsPerPass = std::map<std::string, int>;

struct ProfilerData
{
    // CPU Frametime
    std::map<std::string, Ref<Timer>> frameTimeTimers_ = {};
    std::vector<std::string> frameTimeOrder_;

    // GPU Frametime
    GPUTimer gpuTimer_;

    // Draw calls
    DrawCallsPerPass drawCallsPerPass = {};
};

class Profiler
{
  public:
    // CPU timing
    static void beginCPURegion(const std::string& regionName);
    static void endCPURegion(const std::string& regionName);

    // GPU timing
    static void beginGPURegion(const std::string& regionName);
    static void endGPURegion(const std::string& regionName);
    static void swapGPUTimerBuffers();

    // Draw call tracking
    static void registerDrawCall(const std::string& passName);
    static void resetStats();

    // Getters
    static FrameTimePerRegion getFrameTimeList();
    static GPUTimePerRegion getGPUTimeList();
    static DrawCallsPerPass getDrawCallList();

  private:
};

} // namespace Engine
