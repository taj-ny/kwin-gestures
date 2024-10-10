#include "inputfilter.h"
#include "wayland_server.h"

namespace KWin
{

bool GestureInputEventFilter::swipeGestureBegin(int fingerCount, std::chrono::microseconds time)
{
    Q_UNUSED(time)

    if (waylandServer()->isScreenLocked())
        return false;

    m_touchpadGestureFingerCount = fingerCount;
    if (m_touchpadGestureFingerCount < 3)
        return false;

    return m_touchpadGestureRecognizer.swipeGestureBegin(fingerCount);
}

bool GestureInputEventFilter::swipeGestureUpdate(const QPointF &delta, std::chrono::microseconds time)
{
    Q_UNUSED(time)

    if (waylandServer()->isScreenLocked() || m_touchpadGestureFingerCount < 3)
        return false;

    return m_touchpadGestureRecognizer.swipeGestureUpdate(delta);
}

bool GestureInputEventFilter::swipeGestureEnd(std::chrono::microseconds time)
{
    Q_UNUSED(time)

    if (waylandServer()->isScreenLocked() || m_touchpadGestureFingerCount < 3)
        return false;

    return m_touchpadGestureRecognizer.swipeGestureEnd();
}

bool GestureInputEventFilter::swipeGestureCancelled(std::chrono::microseconds time)
{
    Q_UNUSED(time)

    if (waylandServer()->isScreenLocked() || m_touchpadGestureFingerCount < 3)
        return false;

    return m_touchpadGestureRecognizer.swipeGestureCancelled();
}

bool GestureInputEventFilter::pinchGestureBegin(int fingerCount, std::chrono::microseconds time)
{
    Q_UNUSED(time)

    if (waylandServer()->isScreenLocked())
        return false;

    m_touchpadGestureFingerCount = fingerCount;
    if (m_touchpadGestureFingerCount < 2)
        return false;

    return m_touchpadGestureRecognizer.pinchGestureBegin(fingerCount);
}

bool GestureInputEventFilter::pinchGestureUpdate(qreal scale, qreal angleDelta, const QPointF &delta, std::chrono::microseconds time)
{
    Q_UNUSED(time)

    if (waylandServer()->isScreenLocked() || m_touchpadGestureFingerCount < 2)
        return false;

    return m_touchpadGestureRecognizer.pinchGestureUpdate(scale, angleDelta, delta);
}

bool GestureInputEventFilter::pinchGestureEnd(std::chrono::microseconds time)
{
    Q_UNUSED(time)

    if (waylandServer()->isScreenLocked() || m_touchpadGestureFingerCount < 2)
        return false;

    return m_touchpadGestureRecognizer.pinchGestureEnd();
}

bool GestureInputEventFilter::pinchGestureCancelled(std::chrono::microseconds time)
{
    Q_UNUSED(time)

    if (waylandServer()->isScreenLocked() || m_touchpadGestureFingerCount < 2)
        return false;

    return m_touchpadGestureRecognizer.pinchGestureCancelled();
}

void GestureInputEventFilter::registerTouchpadGesture(SwipeGesture *gesture)
{
    m_touchpadGestureRecognizer.registerGesture(gesture);
}

void GestureInputEventFilter::registerTouchpadGesture(PinchGesture *gesture)
{
    m_touchpadGestureRecognizer.registerGesture(gesture);
}

void GestureInputEventFilter::unregisterGestures()
{
    m_touchpadGestureRecognizer.unregisterGestures();
}

}

#include "moc_inputfilter.cpp"