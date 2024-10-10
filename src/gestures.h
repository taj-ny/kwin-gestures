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

Q_SIGNALS:
    void started();
    void triggered();
    void cancelled();
};

class SwipeGesture : public Gesture
{
public:
    SwipeGesture(SwipeDirection direction, int minimumFingerCount, int maximumFingerCount);

    SwipeDirection direction() const {
        return m_direction;
    }

    int minimumFingerCount() const {
        return m_minimumFingerCount;
    }

    int maximumFingerCount() const {
        return m_maximumFingerCount;
    }

private:
    SwipeDirection m_direction;
    int m_minimumFingerCount;
    int m_maximumFingerCount;
};

};