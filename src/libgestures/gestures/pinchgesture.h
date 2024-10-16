#pragma once

#include "gesture.h"
#include <QRegularExpression>

enum PinchDirection
{
    Any,
    Contracting,
    Expanding
};

class PinchGesture : public Gesture
{
public:
    PinchGesture(bool triggerWhenThresholdReached, uint minimumFingers, uint maximumFingers, bool triggerOneActionOnly, qreal threshold, PinchDirection direction);

    PinchDirection direction() const { return m_direction; }

    bool thresholdReached(const qreal &scale) const override;
private:
    const PinchDirection m_direction;
};