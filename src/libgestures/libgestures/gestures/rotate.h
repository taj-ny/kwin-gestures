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
    bool satisfiesUpdateConditions(const GestureSpeed &speed, const uint32_t &direction) const override;
    void setDirection(const RotateDirection &direction);

    GestureType type() const override;

private:
    RotateDirection m_direction = RotateDirection::Clockwise;
};

}