#pragma once

#include "gesture.h"

namespace libgestures
{

enum class RotateDirection {
    Any,
    Clockwise,
    Counterclockwise
};

class RotateGesture : public Gesture
{
public:
    bool satisfiesUpdateConditions(const GestureSpeed &speed, const RotateDirection &direction) const;
    void setDirection(const RotateDirection &direction);

private:
    RotateDirection m_direction = RotateDirection::Clockwise;
};

}