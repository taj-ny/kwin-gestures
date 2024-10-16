#pragma once

#include <cstdint>

class VirtualInputDevice
{
public:
    VirtualInputDevice() = default;
    virtual ~VirtualInputDevice() = default;

    virtual void keyboardPress(const uint32_t &key) const { };
    virtual void keyboardRelease(const uint32_t &key) const { };
};