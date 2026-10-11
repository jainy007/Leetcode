// Contract checks. A violated contract is a programming error: report and stop (fail-safe),
// the platform's execution manager / watchdog is responsible for the reaction.
#pragma once

namespace ea {

[[noreturn]] void panic(const char* file, int line, const char* expr) noexcept;

}  // namespace ea

#define EA_ASSERT(cond)                            \
    do {                                           \
        if (!(cond)) {                             \
            ::ea::panic(__FILE__, __LINE__, #cond); \
        }                                          \
    } while (false)
