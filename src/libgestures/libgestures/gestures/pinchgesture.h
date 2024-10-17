#pragma once

#include "gesture.h"
#include <QRegularExpression>

namespace libgestures
{

enum PinchDirection
{
    Any,
    Contracting,
    Expanding
};

class PinchGesture : public Gesture
{
public:
    PinchDirection direction() const { return m_direction; }

    bool thresholdReached(const qreal &scale) const override;

    void setDirection(const PinchDirection &direction);
private:
    PinchDirection m_direction = PinchDirection::Any;
};

} // namespace libgestures