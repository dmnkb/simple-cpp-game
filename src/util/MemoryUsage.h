#pragma once

#include <cstddef>

#if defined(_WIN32)
#include <psapi.h>
#include <windows.h>
#elif defined(__APPLE__)
#include <mach/mach.h>
#elif defined(__linux__)
#include <cstdio>
#include <unistd.h>
#endif

namespace Engine
{

// TODO: Distinguish between stack and heap memory usage?

inline std::size_t GetProcessMemoryUsage()
{
#if defined(_WIN32)

    PROCESS_MEMORY_COUNTERS info;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info)))
        return static_cast<std::size_t>(info.WorkingSetSize);
    return 0;

#elif defined(__APPLE__)

    mach_task_basic_info info;
    mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, reinterpret_cast<task_info_t>(&info), &count) != KERN_SUCCESS)
    {
        return 0;
    }
    return static_cast<std::size_t>(info.resident_size);

#elif defined(__linux__)

    long rss = 0L;
    FILE* fp = std::fopen("/proc/self/statm", "r");
    if (!fp)
        return 0;
    if (std::fscanf(fp, "%*s%ld", &rss) != 1)
    {
        std::fclose(fp);
        return 0;
    }
    std::fclose(fp);
    return static_cast<std::size_t>(rss) * static_cast<std::size_t>(::sysconf(_SC_PAGESIZE));

#else
    return 0; // unsupported platform
#endif
}

} // namespace Engine
