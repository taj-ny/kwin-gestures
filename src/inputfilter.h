#pragma once

#include "gesturerecognizer.h"
#include "input.h"

namespace KWin
{

class GestureInputEventFilter : public QObject, public InputEventFilter
{
    Q_OBJECT

public:
    bool swipeGestureBegin(int fingerCount, std::chrono::microseconds time) override;
    bool swipeGestureUpdate(const QPointF &delta, std::chrono::microseconds time) override;
    bool swipeGestureEnd(std::chrono::microseconds time) override;
    bool swipeGestureCancelled(std::chrono::microseconds time) override;

    bool pinchGestureBegin(int fingerCount, std::chrono::microseconds time) override;
    bool pinchGestureUpdate(qreal scale, qreal angleDelta, const QPointF &delta, std::chrono::microseconds time) override;
    bool pinchGestureEnd(std::chrono::microseconds time) override;
    bool pinchGestureCancelled(std::chrono::microseconds time) override;

    void registerTouchpadGesture(SwipeGesture *gesture);
    void registerTouchpadGesture(PinchGesture *gesture);
    void unregisterGestures();

private:
    GestureRecognizer m_touchpadGestureRecognizer;
    uint m_touchpadGestureFingerCount = 0;
};

}