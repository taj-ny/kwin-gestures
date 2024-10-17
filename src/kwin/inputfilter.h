#pragma once

#include "input.h"
#include "libgestures/gestures/gesturerecognizer.h"
#include <QTimer>

// CLion doesn't support @copydoc
/**
 * Installed before GlobalShortcutFilter. Prevents it from receiving input events for which a custom gesture added by
 * the user has been recognized.
 *
 * @remark If KWin version <=6.1.90, this filter is installed as the first filter. For this reason, all methods that
 * process events must not do that if the session is locked. On later versions, it's installed right before
 * GlobalShortcutFilter
 *
 * @returns All methods that process events should return @c true to stop further event processing, @c false to pass to
 * next filter.
 */
class GestureInputEventFilter : public QObject, public KWin::InputEventFilter
{
    Q_OBJECT
public:
    GestureInputEventFilter();

    void registerTouchpadGesture(std::shared_ptr<libgestures::HoldGesture> gesture);
    void registerTouchpadGesture(std::shared_ptr<libgestures::PinchGesture> gesture);
    void registerTouchpadGesture(std::shared_ptr<libgestures::SwipeGesture> gesture);

    /**
     * Unregisters gestures for all devices.
     */
    void unregisterGestures();

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
private:
    libgestures::GestureRecognizer m_touchpadGestureRecognizer;
    /**
     * Amount of fingers currently on the touchpad.
     */
    uint8_t m_touchpadGestureFingerCount = 0;
    QTimer m_touchpadHoldGestureTimer;
};