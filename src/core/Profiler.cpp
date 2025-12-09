#include "core/Profiler.h"
#include "pch.h"

namespace Engine
{
static ProfilerData s_profilerData = {};

// MARK: CPU Timing
void Profiler::beginCPURegion(const std::string& regionName)
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

void Profiler::endCPURegion(const std::string& regionName)
{
    auto it = s_profilerData.frameTimeTimers_.find(regionName);
    if (it == s_profilerData.frameTimeTimers_.end())
    {
        std::print("Timer for {} missing! (Profiler::end())", regionName);
        return;
    }

    it->second->stop();
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

// MARK: GPU Timing
void Profiler::beginGPURegion(const std::string& regionName)
{
    s_profilerData.gpuTimer_.begin(regionName);
}

void Profiler::endGPURegion(const std::string& regionName)
{
    s_profilerData.gpuTimer_.end(regionName);
}

void Profiler::swapGPUTimerBuffers()
{
    s_profilerData.gpuTimer_.swapBuffers();
}

GPUTimePerRegion Profiler::getGPUTimeList()
{
    const auto& results = s_profilerData.gpuTimer_.getResults();
    return GPUTimePerRegion(results.begin(), results.end());
}

void Profiler::registerDrawCall(const std::string& passName)
{
    s_profilerData.drawCallsPerPass[passName]++;
}

void Profiler::resetStats()
{
    s_profilerData.drawCallsPerPass.clear();
}

// MARK: Draw Call Stats
DrawCallsPerPass Profiler::getDrawCallList()
{
    return s_profilerData.drawCallsPerPass;
}

} // namespace Engine
