#pragma once

#include <cstdint>

namespace libgestures
{

/**
 * Provides access to input.
 */
class Input
{
public:
    Input() = default;
    virtual ~Input() = default;

    /**
     * @param state True to press, false to release.
     */
    virtual void keyboardKey([[maybe_unused]] const uint32_t &key, [[maybe_unused]] const bool &state) { };
};

}