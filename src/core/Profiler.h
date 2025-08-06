#pragma once

#include "Timer.h"
#include "core/Core.h"

namespace Engine
{

struct ProfilerData
{
    std::map<std::string, Ref<Timer>> timers = {};
    std::vector<std::string> order;
    std::map<std::string, int> drawCallsPerPass = {};
};

class Profiler
{
  public:
    static void beginRegion(const std::string& regionName);
    static void endRegion(const std::string& regionName);

    static void registerDrawCall(const std::string& passName);
    static void resetStats();

    static std::vector<std::pair<std::string, double>> getAll();

  private:
};

} // namespace Engine
