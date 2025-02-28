#pragma once

#include "input.h"
#include "libgestures/gestures/gesturerecognizer.h"
#include <QTimer>

/**
 * Installed before GlobalShortcutFilter. Prevents it from receiving input events for which a custom gesture added by
 * the user has been recognized and handled.
 *
 * @remark If KWin version <=6.1.90, this filter is installed as the first one. For this reason, all methods that
 * process events must not do anything if the session is locked and must pass the event to the next filter. On later
 * versions, it's installed right before GlobalShortcutFilter.
 *
 * @returns All methods that process events should return @c true to stop further event processing, @c false to pass to
 * next filter.
 */
class GestureInputEventFilter : public QObject, public KWin::InputEventFilter
{
    Q_OBJECT
public:
    GestureInputEventFilter();

    void setTouchpadGestureRecognizer(const std::shared_ptr<libgestures::GestureRecognizer> &gestureRecognizer);

    bool holdGestureBegin(int fingerCount, std::chrono::microseconds time) override;
    void holdGestureUpdate(const qreal &delta);
    bool holdGestureEnd(std::chrono::microseconds time) override;
    bool holdGestureCancelled(std::chrono::microseconds time) override;

    bool swipeGestureBegin(int fingerCount, std::chrono::microseconds time) override;
    bool swipeGestureUpdate(const QPointF &delta, std::chrono::microseconds time) override;
    bool swipeGestureEnd(std::chrono::microseconds time) override;
    bool swipeGestureCancelled(std::chrono::microseconds time) override;

    bool pinchGestureBegin(int fingerCount, std::chrono::microseconds time) override;
    bool pinchGestureUpdate(qreal scale, qreal angleDelta, const QPointF &delta, std::chrono::microseconds time) override;
    bool pinchGestureEnd(std::chrono::microseconds time) override;
    bool pinchGestureCancelled(std::chrono::microseconds time) override;

#ifdef KWIN_6_3_OR_GREATER
    bool pointerAxis(KWin::PointerAxisEvent *event) override;
#else
    bool wheelEvent(KWin::WheelEvent *event) override;
#endif

private:
    std::shared_ptr<libgestures::GestureRecognizer> m_touchpadGestureRecognizer = std::make_shared<libgestures::GestureRecognizer>();
    QTimer m_touchpadHoldGestureTimer;
    QTimer m_scrollTimer;

    bool m_pinchGestureActive = false;
};