#pragma once

#include "gestures.h"
#include <QObject>
#include <QPointF>

namespace KWin
{

class GestureRecognizer
{

public:
    GestureRecognizer() = default;
    ~GestureRecognizer();

    void registerGesture(SwipeGesture *gesture);
    void registerGesture(PinchGesture *gesture);
    void unregisterGestures();

    bool swipeGestureBegin(uint fingerCount);
    bool swipeGestureUpdate(const QPointF &delta);
    bool swipeGestureEnd(bool resetHasActiveTriggeredGesture = true);
    bool swipeGestureCancelled();

    bool pinchGestureBegin(int fingerCount);
    bool pinchGestureUpdate(qreal scale, qreal angleDelta, const QPointF &delta);
    bool pinchGestureEnd();
    bool pinchGestureCancelled();

private:
    QList<SwipeGesture*> m_swipeGestures;
    QList<SwipeGesture*> m_activeSwipeGestures;
    bool m_hasActiveTriggeredSwipeGesture = false;

    QList<PinchGesture*> m_pinchGestures;
    QList<PinchGesture*> m_activePinchGestures;


    uint m_currentFingerCount = 0;
    Axis m_currentSwipeAxis = Axis::None;
    QPointF m_currentDelta;
    qreal m_currentScale = 1;
};

};