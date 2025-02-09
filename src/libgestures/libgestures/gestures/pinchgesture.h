#pragma once

#include "gesture.h"
#include <QRegularExpression>

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
    PinchDirection direction() const
    {
        return m_direction;
    }

    void setDirection(const PinchDirection &direction);

    qreal getRealDelta(const QPointF &delta) const override;

private:
    PinchDirection m_direction = PinchDirection::Any;
};

}