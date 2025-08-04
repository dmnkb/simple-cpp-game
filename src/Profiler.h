#pragma once

#include "Timer.h"
#include "pch.h"

namespace Engine
{
struct ProfilerData
{
    std::map<std::string, Ref<Timer>> timers = {};
    std::vector<std::string> order;
};

class Profiler
{
  public:
    static void beginRegion(const std::string& regionName);
    static void endRegion(const std::string& regionName);

    static std::vector<std::pair<std::string, double>> getAll();

  private:
};
} // namespace Engine
