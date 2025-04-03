#include "touchpadtriggerhandler.h"

namespace libgestures
{

TouchpadTriggerHandler::TouchpadTriggerHandler()
{
    m_scrollTimeoutTimer.setSingleShot(true);
    connect(&m_scrollTimeoutTimer, &QTimer::timeout, this, [this] {
        endTriggers(TriggerType::StrokeSwipe);
    });
}

bool TouchpadTriggerHandler::holdBegin(const uint8_t &fingers)
{
    return activateTriggers(TriggerType::Press, fingers);
}

bool TouchpadTriggerHandler::holdEnd()
{
    return endTriggers(TriggerType::Press);
}

bool TouchpadTriggerHandler::holdCancel()
{
    return cancelTriggers(TriggerType::Press);
}

bool TouchpadTriggerHandler::pinchBegin(const uint8_t &fingers)
{
    return activateTriggers(TriggerType::PinchRotate, fingers);
}

bool TouchpadTriggerHandler::pinchUpdate(const qreal &scale, const qreal &angleDelta, const QPointF &delta)
{
    return pinch(scale, angleDelta, delta);
}

bool TouchpadTriggerHandler::pinchEnd()
{
    return endTriggers(TriggerType::PinchRotate);
}

bool TouchpadTriggerHandler::pinchCancel()
{
    return cancelTriggers(TriggerType::PinchRotate);
}

bool TouchpadTriggerHandler::swipeBegin(const uint8_t &fingers)
{
    return activateTriggers(TriggerType::StrokeSwipe, fingers);
}

bool TouchpadTriggerHandler::swipeUpdate(const QPointF &delta)
{
    return updateMotion(delta);
}

bool TouchpadTriggerHandler::swipeEnd()
{
    return endTriggers(TriggerType::StrokeSwipe);
}

bool TouchpadTriggerHandler::swipeCancel()
{
    return cancelTriggers(TriggerType::StrokeSwipe);
}

bool TouchpadTriggerHandler::scroll(const qreal &delta, const Qt::Orientation &orientation, const qreal &inverted)
{
    auto motionDelta = orientation == Qt::Orientation::Horizontal
        ? QPointF(delta, 0)
        : QPointF(0, delta);
    if (inverted) {
        motionDelta *= -1;
    }

    if (!m_scrollTimeoutTimer.isActive()) {
        activateTriggers(TriggerType::StrokeSwipe, 2);
    }
    m_scrollTimeoutTimer.start(m_scrollTimeout);
    if (updateMotion(motionDelta)) {
        return true;
    }
    m_scrollTimeoutTimer.stop();
    return false;
}

void TouchpadTriggerHandler::setSwipeDeltaMultiplier(const qreal &multiplier)
{
    m_swipeDeltaMultiplier = multiplier;
}

void TouchpadTriggerHandler::setScrollTimeout(const qreal &timeout)
{
    m_scrollTimeout = timeout;
}

}