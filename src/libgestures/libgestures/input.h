#pragma once

#include <memory>

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

    virtual void keyboardPress([[maybe_unused]] const uint32_t &key) { };
    virtual void keyboardRelease([[maybe_unused]] const uint32_t &key) { };

    static Input *implementation()
    {
        return s_implementation.get();
    }
    static void setImplementation(Input *implementation);

private:
    static std::unique_ptr<Input> s_implementation;
};

}