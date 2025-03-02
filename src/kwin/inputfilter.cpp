#include "inputfilter.h"

#include "utils.h"

#ifndef KWIN_6_3_OR_GREATER
#include "core/inputdevice.h"
#endif
#include "input_event.h"
#include "input_event_spy.h"
#include "wayland_server.h"

static uint32_t s_scrollTimeout = 100;
static qreal s_holdDelta = 5;

GestureInputEventFilter::GestureInputEventFilter()
#ifdef KWIN_6_2_OR_GREATER
    : KWin::InputEventFilter(KWin::InputFilterOrder::TabBox)
#endif
{
    m_touchpadHoldGestureTimer.setTimerType(Qt::PreciseTimer);
    connect(&m_touchpadHoldGestureTimer, &QTimer::timeout, this, [this]() {
        holdGestureUpdate(s_holdDelta);
    });

    m_scrollTimer.setSingleShot(true);
    connect(&m_scrollTimer, &QTimer::timeout, this, [this]() {
        swipeGestureEnd(timestamp());
    });
}

void GestureInputEventFilter::setMouseGestureRecognizer(const std::shared_ptr<libgestures::GestureRecognizer> &gestureRecognizer)
{
    m_mouseGestureRecognizer = gestureRecognizer;
}

void GestureInputEventFilter::setTouchpadGestureRecognizer(const std::shared_ptr<libgestures::GestureRecognizer> &gestureRecognizer)
{
    m_touchpadGestureRecognizer = gestureRecognizer;
}

bool GestureInputEventFilter::holdGestureBegin(int fingerCount, std::chrono::microseconds time)
{
    Q_UNUSED(time)

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    m_touchpadGestureRecognizer->holdGestureBegin(fingerCount);
    m_touchpadHoldGestureTimer.start(s_holdDelta);
    return false;
}

void GestureInputEventFilter::holdGestureUpdate(const qreal &delta)
{
#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return;
#endif

    auto endedPrematurely = false;
    m_touchpadGestureRecognizer->holdGestureUpdate(delta, endedPrematurely);
    if (!endedPrematurely)
        return;

    holdGestureEnd(timestamp());
}

bool GestureInputEventFilter::holdGestureEnd(std::chrono::microseconds time)
{
    Q_UNUSED(time)

    m_touchpadHoldGestureTimer.stop();

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    if (m_touchpadGestureRecognizer->holdGestureEnd()) {
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

    m_touchpadHoldGestureTimer.stop();

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    m_touchpadGestureRecognizer->holdGestureCancel();
    return false;
}

bool GestureInputEventFilter::swipeGestureBegin(int fingerCount, std::chrono::microseconds time)
{
    Q_UNUSED(time)

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    m_touchpadGestureRecognizer->swipeGestureBegin(fingerCount);
    return false;
}

bool GestureInputEventFilter::swipeGestureUpdate(const QPointF &delta, std::chrono::microseconds time)
{
    Q_UNUSED(time)

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    auto endedPrematurely = false;
    const auto filter = m_touchpadGestureRecognizer->swipeGestureUpdate(delta, endedPrematurely);
    if (endedPrematurely) {
        swipeGestureEnd(time);
        return true;
    }

    return filter;
}

bool GestureInputEventFilter::swipeGestureEnd(std::chrono::microseconds time)
{
    Q_UNUSED(time)

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    if (m_touchpadGestureRecognizer->swipeGestureEnd()) {
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

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    m_touchpadGestureRecognizer->swipeGestureCancel();
    return false;
}

bool GestureInputEventFilter::pinchGestureBegin(int fingerCount, std::chrono::microseconds time)
{
    Q_UNUSED(time)

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    m_pinchGestureActive = true;
    m_touchpadGestureRecognizer->pinchGestureBegin(fingerCount);
    return false;
}

bool GestureInputEventFilter::pinchGestureUpdate(qreal scale, qreal angleDelta, const QPointF &delta, std::chrono::microseconds time)
{
    Q_UNUSED(time)

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    // Two-finger pinch gestures may be at first incorrectly interpreted by libinput as scrolling. Libinput does
    // eventually correctly determine the gesture after a few events, but it doesn't send the GESTURE_PINCH_BEGIN event.
    if (!m_pinchGestureActive) {
        pinchGestureBegin(2, time);
    }

    auto endedPrematurely = false;
    const auto filter = m_touchpadGestureRecognizer->pinchGestureUpdate(scale, angleDelta, delta, endedPrematurely);
    if (endedPrematurely) {
        pinchGestureEnd(time);
        return true;
    }

    return filter;
}

bool GestureInputEventFilter::pinchGestureEnd(std::chrono::microseconds time)
{
    Q_UNUSED(time)

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    m_pinchGestureActive = false;
    if (m_touchpadGestureRecognizer->pinchGestureEnd()) {
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

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    m_pinchGestureActive = false;
    m_touchpadGestureRecognizer->pinchGestureCancel();
    return false;
}

bool GestureInputEventFilter::pointerMotion(KWin::PointerMotionEvent *event)
{
    const auto device = event->device;
    if (!device->isPointer() || device->isTouch() || device->isTouchpad()) {
        return false;
    }

    bool _ = false;
    return m_mouseGestureRecognizer->swipeGestureUpdate(event->delta, _);
}

bool GestureInputEventFilter::pointerButton(KWin::PointerButtonEvent *event)
{
    const auto device = event->device;
    if (!device->isPointer() || device->isTouch() || device->isTouchpad()) {
        return false;
    }

    if (event->state == PointerButtonStatePressed) {
        m_mouseGestureRecognizer->swipeGestureCancel();
        if (m_mouseGestureRecognizer->swipeGestureBegin(1)) {
            return true;
        }
    } else if (event->state == PointerButtonStateReleased) {
        m_mouseGestureRecognizer->swipeGestureEnd();
    }

    return false;
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

    if (!device->isTouchpad()) {
        return false;
    }

    if (!m_scrollTimer.isActive()) {
        swipeGestureBegin(2, timestamp());
    }
    m_scrollTimer.stop();
    m_scrollTimer.start(s_scrollTimeout);

    auto delta = orientation == Qt::Orientation::Horizontal
        ? QPointF(eventDelta, 0)
        : QPointF(0, eventDelta);
    if (inverted) {
        delta *= -1;
    }
    if (swipeGestureUpdate(delta, timestamp())) {
        return true;
    }

    return false;
}

#include "moc_inputfilter.cpp"