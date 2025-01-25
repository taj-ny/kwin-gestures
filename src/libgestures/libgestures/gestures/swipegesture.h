#pragma once

#include "gesture.h"

namespace libgestures
{

enum class SwipeDirection
{
    Left,
    Right,
    Down,
    Up,
    Vertical,
    Horizontal
};

class SwipeGesture : public Gesture
{
public:
    SwipeDirection direction() const { return m_direction; }

    void setDirection(const SwipeDirection &direction);
private:
    SwipeDirection m_direction = SwipeDirection::Left;
};

}