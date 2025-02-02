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
    virtual void keyboardKey(const uint32_t &key, const bool &state) { };

    /**
     * @param state True to press, false to release.
     */
    virtual void mouseButton(const uint32_t &button, const bool &state) { };
    virtual void mouseMoveAbsolute(const QPointF &pos) { };
    virtual void mouseMoveRelative(const QPointF &pos) { };

    static Input *implementation()
    {
        return s_implementation.get();
    }
    static void setImplementation(Input *implementation);

private:
    static std::unique_ptr<Input> s_implementation;
};

}