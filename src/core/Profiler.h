#pragma once

#include "core/Core.h"
#include "core/Timer.h"

namespace Engine
{

using FrameTimePerRegion = std::vector<std::pair<std::string, double>>;
using DrawCallsPerPass = std::map<std::string, int>;

struct ProfilerData
{
    // Frametime
    std::map<std::string, Ref<Timer>> frameTimeTimers_ = {};
    std::vector<std::string> frameTimeOrder_;

    // Draw calls
    DrawCallsPerPass drawCallsPerPass = {};
};

class Profiler
{
  public:
    static void beginRegion(const std::string& regionName);
    static void endRegion(const std::string& regionName);

    static void registerDrawCall(const std::string& passName);
    static void resetStats();

    static FrameTimePerRegion getFrameTimeList();
    static DrawCallsPerPass getDrawCallList();

  private:
};

} // namespace Engine
