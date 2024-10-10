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

void GestureInputEventFilter::registerTouchpadGesture(SwipeGesture *gesture)
{
    m_touchpadGestureRecognizer.registerGesture(gesture);
}

void GestureInputEventFilter::unregisterGestures()
{
    m_touchpadGestureRecognizer.unregisterGestures();
}

}

#include "moc_inputfilter.cpp"