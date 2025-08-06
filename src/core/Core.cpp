#include "Core.h"
#include <cstdarg>
#include <cstdlib>
#include <iostream>
#include <print>

namespace core
{

void Assert(const char* expression, const char* file, int32_t line, const char* format, ...)
{
    std::print(stderr, "\nAssertion failed!\n");
    std::print(stderr, "File: {}:{}\n", file, line);
    std::print(stderr, "Expression: {}\n", expression);

    if (format)
    {
        std::print(stderr, "📣 Message: ");
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
        printf("\n");
    }

    std::print(stderr, "\nPress [Enter] to exit...\n");

    // Optional: wait for input (only in interactive consoles)
    std::cin.get();

    // Optional: debug break
#if defined(_MSC_VER)
    __debugbreak();
#elif defined(__GNUC__) || defined(__clang__)
    __builtin_trap();
#else
    std::abort();
#endif
}

} // namespace core
