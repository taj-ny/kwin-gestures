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

    virtual void keyboardPress([[maybe_unused]] const uint32_t &key) { };
    virtual void keyboardRelease([[maybe_unused]] const uint32_t &key) { };

    virtual void mouseMoveAbsolute([[maybe_unused]] const QPointF &pos) { };
    virtual void mouseMoveRelative([[maybe_unused]] const QPointF &pos) { };
    virtual void mousePress([[maybe_unused]] const uint32_t &button) { };
    virtual void mouseRelease([[maybe_unused]] const uint32_t &button) { };
};

}