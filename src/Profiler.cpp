#include "Profiler.h"

namespace Engine
{
static ProfilerData s_profilerData = {};

void Profiler::beginRegion(const std::string& regionName)
{
    auto it = s_profilerData.timers.find(regionName);
    if (it == s_profilerData.timers.end())
    {
        auto newTimer = CreateRef<Timer>();
        newTimer->start();
        s_profilerData.timers[regionName] = newTimer;
        s_profilerData.order.push_back(regionName);
    }
    else
    {
        it->second->start();
    }
}

void Profiler::endRegion(const std::string& regionName)
{
    auto it = s_profilerData.timers.find(regionName);
    if (it == s_profilerData.timers.end())
    {
        std::print("Timer for {} missing! (Profiler::end())", regionName);
        return;
    }

    it->second->stop();
}

std::vector<std::pair<std::string, double>> Profiler::getAll()
{
    std::vector<std::pair<std::string, double>> regions;

    for (const auto& name : s_profilerData.order)
    {
        const auto& timer = s_profilerData.timers.at(name);
        if (timer)
            regions.emplace_back(name, timer->elapsedMilliseconds());
    }

    return regions;
}
} // namespace Engine
