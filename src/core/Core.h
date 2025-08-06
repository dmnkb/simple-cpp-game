#pragma once

#include "stdint.h"

namespace Engine
{

// See: https://github.com/TheCherno/Hazel
template <typename T> using Scope = std::unique_ptr<T>;
template <typename T, typename... Args> constexpr Scope<T> CreateScope(Args&&... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T> using Ref = std::shared_ptr<T>;
template <typename T, typename... Args> constexpr Ref<T> CreateRef(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

// See: https://github.com/kperdlich/wiicraft/blob/master/src/engine/Core.h
void Assert(const char* expression, const char* file, int32_t line, const char* format, ...);

#define ASSERT_TEXT(e, format, ...) ((e) ? (void)0 : core::Assert(#e, __FILE__, __LINE__, format, ##__VA_ARGS__))

#define ASSERT(e) ((e) ? (void)0 : core::Assert(#e, __FILE__, __LINE__, nullptr))

} // namespace Engine