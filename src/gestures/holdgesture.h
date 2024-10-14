#pragma once

#include "gesture.h"
#include <QTimer>

class HoldGesture : public Gesture
{
    Q_OBJECT
public:
    HoldGesture(InputDeviceType device, bool triggerWhenThresholdReached, uint minimumFingers, uint maximumFingers, uint threshold, bool triggerOneActionOnly);

    void cancelled() override;
    void started() override;
    void triggered() override;

    bool thresholdReached(const std::chrono::milliseconds &time) const;
Q_SIGNALS:
    void triggeredByTimer();
private:
    QTimer m_timer;
    const uint m_threshold;
};