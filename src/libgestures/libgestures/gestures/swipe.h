#pragma once

#include "gesture.h"

namespace libgestures
{

enum class SwipeDirection {
    Any,
    Left,
    Right,
    Down,
    Up,
    LeftRight,
    UpDown
};

class SwipeGesture : public Gesture
{
public:
    bool satisfiesUpdateConditions(const GestureSpeed &speed, const uint32_t &direction) const override;
    void setDirection(const SwipeDirection &direction);

    GestureType type() const override;

private:
    SwipeDirection m_direction = SwipeDirection::Left;
};

}