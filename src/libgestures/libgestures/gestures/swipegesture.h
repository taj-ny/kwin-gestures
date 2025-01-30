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

    LeftUp,
    LeftDown,
    RightUp,
    RightDown,

    // Bi-directional
    LeftRight,
    UpDown,

    LeftUpRightDown,
    LeftDownRightUp,

    None
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