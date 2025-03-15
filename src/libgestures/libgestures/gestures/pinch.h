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
    bool satisfiesUpdateConditions(const GestureSpeed &speed, const PinchDirection &direction) const;
    void setDirection(const PinchDirection &direction);

private:
    PinchDirection m_direction = PinchDirection::Any;
};

}