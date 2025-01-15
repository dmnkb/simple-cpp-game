#pragma once

#include <algorithm>
#include <deque>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>

// Thanks to: https://github.com/TheCherno/Hazel
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

#define Is(type, variant) std::holds_alternative<type>(variant)
#define Get(type, variant) std::get<type>(variant)
