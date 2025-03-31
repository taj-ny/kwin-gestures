#pragma once

#include "input.h"

#include "impl/kwininput.h"

#include "libgestures/gestures/handler.h"

#include <QTimer>

/**
 * Captures input events, forwards them to the proper instance of GestureHandler, and blocks them if necessary.
 *
 * Installed before GlobalShortcutFilter, which is responsible for handling touchpad gestures.
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

    void setMouseGestureRecognizer(const std::shared_ptr<libgestures::GestureHandler> &gestureRecognizer);
    void setTouchpadGestureRecognizer(const std::shared_ptr<libgestures::GestureHandler> &gestureRecognizer);

    bool holdGestureBegin(int fingerCount, std::chrono::microseconds time) override;
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
    bool pointerMotion(KWin::PointerMotionEvent *event) override;
    bool pointerButton(KWin::PointerButtonEvent *event) override;
    bool pointerAxis(KWin::PointerAxisEvent *event) override;

    bool keyboardKey(KWin::KeyboardKeyEvent *event) override;
#else
    bool wheelEvent(KWin::WheelEvent *event) override;
#endif

    void recordStroke();

signals:
    void strokeRecordingFinished(const libgestures::Stroke &stroke);

private:
    bool isMouse(const KWin::InputDevice *device) const;

    void finishStrokeRecording();

    std::shared_ptr<libgestures::GestureHandler> m_mouseGestureRecognizer = std::make_shared<libgestures::GestureHandler>();
    std::shared_ptr<libgestures::GestureHandler> m_touchpadGestureRecognizer = std::make_shared<libgestures::GestureHandler>();

    bool m_pinchGestureActive = false;

    bool m_isRecordingStroke = false;
    std::vector<QPointF> m_strokePoints;
    QTimer m_strokeRecordingTimeoutTimer;
};