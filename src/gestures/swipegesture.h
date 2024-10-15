#pragma once

#include "effect/globals.h"
#include "gesture.h"

enum SwipeDirection
{
    Left,
    Right,
    LeftRight,
    Down,
    Up,
    UpDown,
};

class SwipeGesture : public Gesture
{
public:
    SwipeGesture(bool triggerWhenThresholdReached, uint minimumFingers, uint maximumFingers, bool triggerOneActionOnly, qreal threshold, SwipeDirection direction);

    SwipeDirection direction() const { return m_direction; }

    bool thresholdReached(const qreal &delta) const override;
private:
    const SwipeDirection m_direction;
};