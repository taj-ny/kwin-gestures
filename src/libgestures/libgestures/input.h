#pragma once

#include <cstdint>

namespace libgestures
{

class Input
{
public:
    Input() = default;
    virtual ~Input() = default;

    virtual void keyboardPress([[maybe_unused]] const uint32_t &key) const { };
    virtual void keyboardRelease([[maybe_unused]] const uint32_t &key) const { };
};

} // namespace libgestures