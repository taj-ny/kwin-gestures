#pragma once

#include "gesture.h"
#include <QRegularExpression>

namespace libgestures
{

enum class PinchDirection
{
    Any,
    Inward,
    Outward
};

class PinchGesture : public Gesture
{
public:
    PinchDirection direction() const { return m_direction; }

    void setDirection(const PinchDirection &direction);
private:
    PinchDirection m_direction = PinchDirection::Any;
};

}