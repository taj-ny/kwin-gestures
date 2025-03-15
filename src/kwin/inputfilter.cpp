#include "inputfilter.h"

#include "utils.h"

#ifndef KWIN_6_3_OR_GREATER
#include "core/inputdevice.h"
#endif
#include "input_event.h"
#include "input_event_spy.h"
#include "wayland_server.h"

#ifndef KWIN_6_2_OR_GREATER
#define ENSURE_SESSION_UNLOCKED() \
    if (KWin::waylandServer()->isScreenLocked()) { \
        return false;           \
    }
#else
#define ENSURE_SESSION_UNLOCKED()
#endif

GestureInputEventFilter::GestureInputEventFilter()
#ifdef KWIN_6_2_OR_GREATER
    : KWin::InputEventFilter(KWin::InputFilterOrder::TabBox)
#endif
{
}

void GestureInputEventFilter::setMouseGestureRecognizer(const std::shared_ptr<libgestures::GestureHandler> &gestureRecognizer)
{
    m_mouseGestureRecognizer = gestureRecognizer;
    m_mouseGestureRecognizer->setDeviceType(libgestures::DeviceType::Mouse);
}

void GestureInputEventFilter::setTouchpadGestureRecognizer(const std::shared_ptr<libgestures::GestureHandler> &gestureRecognizer)
{
    m_touchpadGestureRecognizer = gestureRecognizer;
    m_touchpadGestureRecognizer->setDeviceType(libgestures::DeviceType::Touchpad);
}

bool GestureInputEventFilter::holdGestureBegin(int fingerCount, std::chrono::microseconds time)
{
    Q_UNUSED(time)
    ENSURE_SESSION_UNLOCKED();

    m_touchpadGestureRecognizer->gestureBegin(libgestures::GestureType::Press, fingerCount);
    return false;
}

bool GestureInputEventFilter::holdGestureEnd(std::chrono::microseconds time)
{
    ENSURE_SESSION_UNLOCKED();

    if (m_touchpadGestureRecognizer->gestureEnd(libgestures::GestureType::Press)) {
        KWin::input()->processSpies([&time](auto &&spy) {
            spy->holdGestureCancelled(time);
        });
        KWin::input()->processFilters([&time](auto &&filter) {
            return filter->holdGestureCancelled(time);
        });
        return true;
    }

    return false;
}

bool GestureInputEventFilter::holdGestureCancelled(std::chrono::microseconds time)
{
    Q_UNUSED(time)
    ENSURE_SESSION_UNLOCKED();

    m_touchpadGestureRecognizer->gestureCancel(libgestures::GestureType::Press);
    return false;
}

bool GestureInputEventFilter::swipeGestureBegin(int fingerCount, std::chrono::microseconds time)
{
    Q_UNUSED(time)
    ENSURE_SESSION_UNLOCKED();

    m_touchpadGestureRecognizer->gestureBegin(libgestures::GestureType::Swipe, fingerCount);
    return false;
}

bool GestureInputEventFilter::swipeGestureUpdate(const QPointF &delta, std::chrono::microseconds time)
{
    ENSURE_SESSION_UNLOCKED();

    auto ended = false;
    const auto filter = m_touchpadGestureRecognizer->swipeGestureUpdate(delta, ended);
    if (ended) {
        swipeGestureEnd(time);
        return true;
    }

    return filter;
}

bool GestureInputEventFilter::swipeGestureEnd(std::chrono::microseconds time)
{
    ENSURE_SESSION_UNLOCKED();

    if (m_touchpadGestureRecognizer->gestureEnd(libgestures::GestureType::Swipe)) {
        KWin::input()->processSpies([&time](auto &&spy) {
            spy->swipeGestureCancelled(time);
        });
        KWin::input()->processFilters([&time](auto &&filter) {
            return filter->swipeGestureCancelled(time);
        });
        return true;
    }

    return false;
}

bool GestureInputEventFilter::swipeGestureCancelled(std::chrono::microseconds time)
{
    Q_UNUSED(time)
    ENSURE_SESSION_UNLOCKED();

    m_touchpadGestureRecognizer->gestureCancel(libgestures::GestureType::Swipe);
    return false;
}

bool GestureInputEventFilter::pinchGestureBegin(int fingerCount, std::chrono::microseconds time)
{
    Q_UNUSED(time)
    ENSURE_SESSION_UNLOCKED();

    m_pinchGestureActive = true;
    m_touchpadGestureRecognizer->gestureBegin(libgestures::GestureType::Pinch | libgestures::GestureType::Rotate, fingerCount);
    return false;
}

bool GestureInputEventFilter::pinchGestureUpdate(qreal scale, qreal angleDelta, const QPointF &delta, std::chrono::microseconds time)
{
    Q_UNUSED(time)
    ENSURE_SESSION_UNLOCKED();

    // Two-finger pinch gestures may be at first incorrectly interpreted by libinput as scrolling. Libinput does
    // eventually correctly determine the gesture after a few events, but it doesn't send the GESTURE_PINCH_BEGIN event.
    if (!m_pinchGestureActive) {
        pinchGestureBegin(2, time);
    }

    auto ended = false;
    const auto filter = m_touchpadGestureRecognizer->pinchGestureUpdate(scale, angleDelta, delta, ended);
    if (ended) {
        pinchGestureEnd(time);
        return true;
    }

    return filter;
}

bool GestureInputEventFilter::pinchGestureEnd(std::chrono::microseconds time)
{
    Q_UNUSED(time)
    ENSURE_SESSION_UNLOCKED();

    m_pinchGestureActive = false;
    if (m_touchpadGestureRecognizer->gestureEnd(libgestures::GestureType::Pinch | libgestures::GestureType::Rotate)) {
        KWin::input()->processSpies([&time](auto &&spy) {
            spy->pinchGestureCancelled(time);
        });
        KWin::input()->processFilters([&time](auto &&filter) {
            return filter->pinchGestureCancelled(time);
        });
        return true;
    }

    return false;
}

bool GestureInputEventFilter::pinchGestureCancelled(std::chrono::microseconds time)
{
    Q_UNUSED(time)
    ENSURE_SESSION_UNLOCKED();

    m_pinchGestureActive = false;
    m_touchpadGestureRecognizer->gestureCancel(libgestures::GestureType::Pinch | libgestures::GestureType::Rotate);
    return false;
}

bool GestureInputEventFilter::pointerMotion(KWin::PointerMotionEvent *event)
{
    ENSURE_SESSION_UNLOCKED();

    const auto device = event->device;
    if (!device || !isMouse(event->device)) {
        return false;
    }

    m_mouseGestureRecognizer->pointerMotion(event->delta);
    return false;
}

bool GestureInputEventFilter::pointerButton(KWin::PointerButtonEvent *event)
{
    ENSURE_SESSION_UNLOCKED();

    if (!event->device || !isMouse(event->device)) {
        return false;
    }

    return m_mouseGestureRecognizer->pointerButton(event->button, event->nativeButton, event->state == PointerButtonStatePressed);
}

#ifdef KWIN_6_3_OR_GREATER
bool GestureInputEventFilter::pointerAxis(KWin::PointerAxisEvent *event)
{
    const auto device = event->device;
    const auto eventDelta = event->delta;
    const auto orientation = event->orientation;
    const auto inverted = event->inverted;
#else
bool GestureInputEventFilter::wheelEvent(KWin::WheelEvent *event)
{
    const auto device = event->device();
    const auto eventDelta = event->delta();
    const auto orientation = event->orientation();
    const auto inverted = event->inverted();
#endif

    ENSURE_SESSION_UNLOCKED();

    const auto mouse = isMouse(device);
    if (!device->isTouchpad() && !mouse) {
        return false;
    }

    auto delta = orientation == Qt::Orientation::Horizontal
        ? QPointF(eventDelta, 0)
        : QPointF(0, eventDelta);
    if (inverted) {
        delta *= -1;
    }

    return mouse ? m_mouseGestureRecognizer->pointerAxis(delta) : m_touchpadGestureRecognizer->pointerAxis(delta);
}

bool GestureInputEventFilter::isMouse(const KWin::InputDevice *device) const
{
    return device->isPointer() && !device->isTouch() && !device->isTouchpad();
}

bool GestureInputEventFilter::keyboardKey(KWin::KeyboardKeyEvent *event) {
    return InputEventFilter::keyboardKey(event);
}

#include "moc_inputfilter.cpp"