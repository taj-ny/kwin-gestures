#pragma once

#include "gesture.h"

namespace libgestures
{

enum class PinchDirection {
    Any,
    In,
    Out
};

class PinchGesture : public Gesture
{
public:
    bool satisfiesUpdateConditions(const GestureSpeed &speed, const uint32_t &direction) const override;
    void setDirection(const PinchDirection &direction);

    GestureType type() const override;

private:
    PinchDirection m_direction = PinchDirection::Any;
};

}