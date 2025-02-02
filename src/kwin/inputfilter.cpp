#include "inputfilter.h"

#include "core/output.h"
#include "input_event_spy.h"
#include "workspace.h"

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

void GestureInputEventFilter::setTouchscreenGestureRecognizer(const std::shared_ptr<libgestures::GestureRecognizer> &gestureRecognizer)
{
    m_touchscreenGestureRecognizer = gestureRecognizer;
}

bool GestureInputEventFilter::holdGestureBegin(int fingerCount, std::chrono::microseconds time)
{
    Q_UNUSED(time)

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked()) {
        return false;
    }
#endif

    m_touchpadGestureFingerCount = fingerCount;
    m_touchpadGestureRecognizer->holdGestureBegin(fingerCount);
    m_touchpadHoldGestureTimer.start(1);
    return false;
}

void GestureInputEventFilter::holdGestureUpdate(const qreal &delta)
{
#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked()) {
        return false;
    }
#endif

    auto endedPrematurely = false;
    m_touchpadGestureRecognizer->holdGestureUpdate(delta, endedPrematurely);
    if (!endedPrematurely) {
        return;
    }

    // TODO proper time
    holdGestureEnd(std::chrono::microseconds(0));
}

bool GestureInputEventFilter::holdGestureEnd(std::chrono::microseconds time)
{
    Q_UNUSED(time)

    m_touchpadHoldGestureTimer.stop();

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked()) {
        return false;
    }
#endif

    if (m_touchpadGestureFingerCount >= 3 && m_touchpadGestureRecognizer->holdGestureEnd()) {
        KWin::input()->processSpies([&time](auto &&spy) {
            spy->holdGestureCancelled(time);
        });
        KWin::input()->processFilters([&time](auto &&filter) {
            return filter->holdGestureCancelled(time);
        });
        return true;
    }

    m_touchpadGestureFingerCount = 0;
    return false;
}

bool GestureInputEventFilter::holdGestureCancelled(std::chrono::microseconds time)
{
    Q_UNUSED(time)

    m_touchpadHoldGestureTimer.stop();

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked()) {
        return false;
    }
#endif

    if (m_touchpadGestureFingerCount >= 2) {
        m_touchpadGestureRecognizer->holdGestureCancel();
    }

    return false;
}

bool GestureInputEventFilter::swipeGestureBegin(int fingerCount, std::chrono::microseconds time)
{
    Q_UNUSED(time)

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked()) {
        return false;
    }
#endif

    m_touchpadGestureFingerCount = fingerCount;
    if (m_touchpadGestureFingerCount >= 3) {
        m_touchpadGestureRecognizer->swipeGestureBegin(fingerCount);
    }

    return false;
}

bool GestureInputEventFilter::swipeGestureUpdate(const QPointF &delta, std::chrono::microseconds time)
{
    Q_UNUSED(time)

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked()) {
        return false;
    }
#endif

    if (m_touchpadGestureFingerCount < 3) {
        return false;
    }

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
    if (KWin::waylandServer()->isScreenLocked()) {
        return false;
    }
#endif

    if (m_touchpadGestureFingerCount >= 3 && m_touchpadGestureRecognizer->swipeGestureEnd()) {
        KWin::input()->processSpies([&time](auto &&spy) {
            spy->swipeGestureCancelled(time);
        });
        KWin::input()->processFilters([&time](auto &&filter) {
            return filter->swipeGestureCancelled(time);
        });
        return true;
    }

    m_touchpadGestureFingerCount = 0;
    return false;
}

bool GestureInputEventFilter::swipeGestureCancelled(std::chrono::microseconds time)
{
    Q_UNUSED(time)

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked()) {
        return false;
    }
#endif

    if (m_touchpadGestureFingerCount >= 3)
        m_touchpadGestureRecognizer->swipeGestureCancel();

    return false;
}

bool GestureInputEventFilter::pinchGestureBegin(int fingerCount, std::chrono::microseconds time)
{
    Q_UNUSED(time)

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked()) {
        return false;
    }
#endif

    m_touchpadGestureFingerCount = fingerCount;
    if (m_touchpadGestureFingerCount >= 2)
        m_touchpadGestureRecognizer->pinchGestureBegin(fingerCount);

    return false;
}

bool GestureInputEventFilter::pinchGestureUpdate(qreal scale, qreal angleDelta, const QPointF &delta, std::chrono::microseconds time)
{
    Q_UNUSED(time)

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked()) {
        return false;
    }
#endif

    if (m_touchpadGestureFingerCount < 2)
        return false;

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
    if (KWin::waylandServer()->isScreenLocked()) {
        return false;
    }
#endif

    if (m_touchpadGestureFingerCount >= 2 && m_touchpadGestureRecognizer->pinchGestureEnd()) {
        KWin::input()->processSpies([&time](auto &&spy) {
            spy->pinchGestureCancelled(time);
        });
        KWin::input()->processFilters([&time](auto &&filter) {
            return filter->pinchGestureCancelled(time);
        });
        return true;
    }

    m_touchpadGestureFingerCount = 0;
    return false;
}

bool GestureInputEventFilter::pinchGestureCancelled(std::chrono::microseconds time)
{
    Q_UNUSED(time)

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked()) {
        return false;
    }
#endif

    if (m_touchpadGestureFingerCount >= 2)
        m_touchpadGestureRecognizer->pinchGestureCancel();

    return false;
}

bool GestureInputEventFilter::touchDown(qint32 id, const QPointF &pos, std::chrono::microseconds time)
{
#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked()) {
        return false;
    }
#endif

    m_hasTouchscreenSwipeGesture = false;
    m_touchscreenSwipeGestureCancelled = false;

    m_touchPoints.insert(id, pos);
    if (m_touchPoints.count() == 1) {
        m_lastTouchDownTime = time;
    } else {
        if (time - m_lastTouchDownTime > std::chrono::milliseconds(250)) {
            m_touchscreenGestureRecognizer->swipeGestureCancel();
            return false;
        }
        m_lastTouchDownTime = time;
        auto output = KWin::workspace()->outputAt(pos);
        auto physicalSize = output->orientateSize(output->physicalSize());
        if (!physicalSize.isValid()) {
            physicalSize = QSize(190, 100);
        }
        float xfactor = physicalSize.width() / (float)output->geometry().width();
        float yfactor = physicalSize.height() / (float)output->geometry().height();
        bool distanceMatch = std::any_of(m_touchPoints.constBegin(), m_touchPoints.constEnd(), [pos, xfactor, yfactor](const auto &point) {
            QPointF p = pos - point;
            return std::abs(xfactor * p.x()) + std::abs(yfactor * p.y()) < 50;
        });
        if (!distanceMatch) {
            m_touchscreenGestureRecognizer->swipeGestureCancel();
            m_touchscreenSwipeGestureCancelled = true;
        }
    }

    return false;
}

bool GestureInputEventFilter::touchMotion(qint32 id, const QPointF &pos, std::chrono::microseconds time)
{
    Q_UNUSED(time)

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked()) {
        return false;
    }
#endif

    if (m_touchscreenSwipeGestureCancelled) {
        return false;
    }
    m_touchscreenGestureRecognizer->swipeGestureBegin(m_touchPoints.count());

    auto output = KWin::workspace()->outputAt(pos);
    const auto physicalSize = output->orientateSize(output->physicalSize());
    const float xfactor = physicalSize.width() / (float)output->geometry().width();
    const float yfactor = physicalSize.height() / (float)output->geometry().height();

    auto &point = m_touchPoints[id];
    const QPointF dist = pos - point;
    const QPointF delta = QPointF(xfactor * dist.x(), yfactor * dist.y());

    auto endedPrematurely = false;
    const auto hasGesture = m_touchscreenGestureRecognizer->swipeGestureUpdate(5 * delta / m_touchPoints.size(), endedPrematurely);
    if (hasGesture && !m_hasTouchscreenSwipeGesture) {
        // Release all touch points immediately, otherwise the hold gesture will activate
        for (const auto touchPoint : m_touchPoints.keys()) {
            KWin::input()->processFilters([this, &touchPoint, &time](auto && f) {
                if (f == this) {
                    return false;
                }
                return f->touchUp(touchPoint, time);
            });
            KWin::input()->processSpies([&touchPoint, &time](auto && s) {
                return s->touchUp(touchPoint, time);
            });
        }
        m_hasTouchscreenSwipeGesture = true;
    }
    if (endedPrematurely) {
        m_touchscreenGestureRecognizer->swipeGestureEnd();
        m_touchscreenSwipeGestureCancelled = true;
    }

    point = pos;
    return m_hasTouchscreenSwipeGesture;
}

bool GestureInputEventFilter::touchUp(qint32 id, std::chrono::microseconds time)
{
    Q_UNUSED(time)

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked()) {
        return false;
    }
#endif

    m_touchPoints.remove(id);
    m_touchscreenGestureRecognizer->swipeGestureCancel();
    m_hasTouchscreenSwipeGesture = false;
    m_touchscreenSwipeGestureCancelled = false;

    return false;
}

#include "moc_inputfilter.cpp"