#pragma once

#include <functional>
#include <optional>
#define UUID_SYSTEM_GENERATOR
#include <stduuid/include/uuid.h>
#include <string>
#include <string_view>

namespace Engine
{

class UUID
{
  public:
    UUID() = default;
    explicit UUID(const uuids::uuid& u) noexcept : m_(u) {}

    // Generate a nil UUID (all zeros)
    static UUID zero() noexcept { return UUID(uuids::uuid{}); }
    static UUID random() { return UUID(generate_impl()); }

    static std::optional<UUID> from_string(std::string_view s)
    {
        if (auto u = uuids::uuid::from_string(s)) return UUID(*u);
        return std::nullopt;
    }

    std::string to_string() const { return uuids::to_string(m_); }
    bool is_nil() const noexcept { return m_.is_nil(); }
    uuids::uuid_version version() const noexcept { return m_.version(); }
    uuids::uuid_variant variant() const noexcept { return m_.variant(); }

    friend bool operator==(const UUID&, const UUID&) noexcept = default;
    friend bool operator<(const UUID& a, const UUID& b) noexcept { return a.m_ < b.m_; }

    const uuids::uuid& value() const noexcept { return m_; }

  private:
    uuids::uuid m_{};

    static uuids::uuid generate_impl()
    {
        thread_local uuids::uuid_system_generator gen{};
        return gen();
    }
};

} // namespace Engine

template <>
struct std::hash<Engine::UUID>
{
    size_t operator()(const Engine::UUID& u) const noexcept { return std::hash<uuids::uuid>{}(u.value()); }
};
