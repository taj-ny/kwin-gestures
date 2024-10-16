#pragma once

#include <cstdint>

class VirtualInputDevice
{
public:
    VirtualInputDevice() = default;
    virtual ~VirtualInputDevice() = default;

    virtual void keyboardPress([[maybe_unused]] const uint32_t &key) const { };
    virtual void keyboardRelease([[maybe_unused]] const uint32_t &key) const { };
};