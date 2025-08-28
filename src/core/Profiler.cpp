#include "Profiler.h"
#include "pch.h"

namespace Engine
{
static ProfilerData s_profilerData = {};

void Profiler::beginRegion(const std::string& regionName)
{
    auto it = s_profilerData.frameTimeTimers_.find(regionName);
    if (it == s_profilerData.frameTimeTimers_.end())
    {
        auto newTimer = CreateRef<Timer>();
        newTimer->start();
        s_profilerData.frameTimeTimers_[regionName] = newTimer;
        s_profilerData.frameTimeOrder_.push_back(regionName);
    }
    else
    {
        it->second->start();
    }
}

void Profiler::endRegion(const std::string& regionName)
{
    auto it = s_profilerData.frameTimeTimers_.find(regionName);
    if (it == s_profilerData.frameTimeTimers_.end())
    {
        std::print("Timer for {} missing! (Profiler::end())", regionName);
        return;
    }

    it->second->stop();
}

void Profiler::registerDrawCall(const std::string& passName)
{
    s_profilerData.drawCallsPerPass[passName]++;
}

void Profiler::resetStats()
{
    s_profilerData.drawCallsPerPass.clear();
}

FrameTimePerRegion Profiler::getFrameTimeList()
{
    FrameTimePerRegion regions;

    for (const auto& name : s_profilerData.frameTimeOrder_)
    {
        const auto& timer = s_profilerData.frameTimeTimers_.at(name);
        if (timer)
            regions.emplace_back(name, timer->elapsedMilliseconds());
    }

    return regions;
}

DrawCallsPerPass Profiler::getDrawCallList()
{
    return s_profilerData.drawCallsPerPass;
}

} // namespace Engine
