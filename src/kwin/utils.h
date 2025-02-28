#pragma once

#include <chrono>

inline std::chrono::microseconds timestamp()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
}