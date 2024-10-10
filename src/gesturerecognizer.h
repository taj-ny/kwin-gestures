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
    void unregisterGestures();

    bool swipeGestureBegin(uint fingerCount);
    bool swipeGestureUpdate(const QPointF &delta);
    bool swipeGestureEnd();
    bool swipeGestureCancelled();

private:
    QList<SwipeGesture*> m_swipeGestures;
    QList<SwipeGesture*> m_activeSwipeGestures;

    uint m_currentFingerCount = 0;
    Axis m_currentSwipeAxis = Axis::None;
    QPointF m_currentDelta;
};

};