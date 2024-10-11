#pragma once

#include "config/gesture.h"
#include <QObject>
#include <QPointF>

enum Axis
{
    Horizontal,
    Vertical,
    None
};

class GestureRecognizer
{
public:
    bool swipeGestureBegin(uint fingerCount);
    bool swipeGestureUpdate(const QPointF &delta);
    bool swipeGestureEnd(bool resetHasActiveTriggeredGesture = true);
    bool swipeGestureCancelled();

    bool pinchGestureBegin(int fingerCount);
    bool pinchGestureUpdate(qreal scale, qreal angleDelta, const QPointF &delta);
    bool pinchGestureEnd();
    bool pinchGestureCancelled();

private:
    QList<std::shared_ptr<SwipeGesture>> m_activeSwipeGestures;
    bool m_hasActiveTriggeredSwipeGesture = false;

    QList<std::shared_ptr<PinchGesture>> m_activePinchGestures;

    uint m_currentFingerCount = 0;
    Axis m_currentSwipeAxis = Axis::None;
    QPointF m_currentDelta;
    qreal m_currentScale = 1;
};