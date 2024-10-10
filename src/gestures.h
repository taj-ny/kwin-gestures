#pragma once

#include "effect/globals.h"
#include <QObject>
#include "options.h"

namespace KWin
{

enum class Axis {
    Horizontal,
    Vertical,
    None
};

class Gesture : public QObject
{
    Q_OBJECT
public:
    int minimumFingerCount() const;
    int maximumFingerCount() const;
    bool triggerAfterReachingThreshold() const;

protected:
    Gesture(int minimumFingerCount, int maximumFingerCount, bool triggerAfterReachingThreshold);

Q_SIGNALS:
    void started();
    void triggered();
    void cancelled();

private:
    int m_minimumFingerCount;
    int m_maximumFingerCount;

    bool m_triggerAfterReachingThreshold;
};

class SwipeGesture : public Gesture
{
public:
    SwipeGesture(SwipeDirection direction, int minimumFingerCount, int maximumFingerCount, bool triggerAfterReachingThreshold, const QPointF &threshold);

    SwipeDirection direction() const;

    bool thresholdReached(const QPointF &delta) const;

private:
    const SwipeDirection m_direction;
    const QPointF m_threshold;
};

class PinchGesture : public Gesture
{
public:
    PinchGesture(PinchDirection direction, int minimumFingerCount, int maximumFingerCount, bool triggerAfterReachingThreshold, qreal threshold);

    PinchDirection direction() const;

    bool thresholdReached(const qreal &scale) const;

private:
    const PinchDirection m_direction;
    const qreal m_threshold;

};

};