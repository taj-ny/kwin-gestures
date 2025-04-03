#pragma once

#include "libgestures/handlers/multitouchmotiontriggerhandler.h"

namespace libgestures
{

class TouchpadTriggerHandler : public MultiTouchMotionTriggerHandler
{
public:
    TouchpadTriggerHandler();

    bool holdBegin(const uint8_t &fingers);
    bool holdEnd();
    bool holdCancel();

    bool pinchBegin(const uint8_t &fingers);
    bool pinchUpdate(const qreal &scale, const qreal &angleDelta, const QPointF &delta);
    bool pinchEnd();
    bool pinchCancel();

    bool swipeBegin(const uint8_t &fingers);
    bool swipeUpdate(const QPointF &delta);
    bool swipeEnd();
    bool swipeCancel();

    bool scroll(const qreal &delta, const Qt::Orientation &orientation, const qreal &inverted);

    void setSwipeDeltaMultiplier(const qreal &multiplier);
    void setScrollTimeout(const qreal &timeout);

private:
    qreal m_scrollTimeout = 100;
    QTimer m_scrollTimeoutTimer;
};

}