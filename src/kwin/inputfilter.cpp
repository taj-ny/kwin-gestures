#include "inputfilter.h"
#include "input_event_spy.h"

#include "wayland_server.h"
#include <utility>

GestureInputEventFilter::GestureInputEventFilter()
#ifdef KWIN_6_2_OR_GREATER
    : KWin::InputEventFilter(KWin::InputFilterOrder::TabBox)
#endif
{
    connect(&m_touchpadHoldGestureTimer, &QTimer::timeout, this, [this]() {
        holdGestureUpdate(1);
    });
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
    m_touchpadHoldGestureTimer.start(1);
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

    // TODO proper time
    holdGestureEnd(std::chrono::microseconds(0));
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

    m_touchpadGestureRecognizer->pinchGestureCancel();
    return false;
}

#include "moc_inputfilter.cpp"