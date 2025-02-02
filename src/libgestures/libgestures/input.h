#pragma once

#include <memory>

#include <QPointF>

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

    /**
     * @param state True to press, false to release.
     */
    virtual void mouseButton([[maybe_unused]] const uint32_t &button, [[maybe_unused]] const bool &state) { };
    virtual void mouseMoveAbsolute([[maybe_unused]] const QPointF &pos) { };
    virtual void mouseMoveRelative([[maybe_unused]] const QPointF &pos) { };

    static Input *implementation()
    {
        return s_implementation.get();
    }
    static void setImplementation(Input *implementation);

private:
    static std::unique_ptr<Input> s_implementation;
};

}