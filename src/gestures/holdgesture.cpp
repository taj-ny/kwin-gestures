#include "holdgesture.h"

HoldGesture::HoldGesture(InputDeviceType device, bool triggerWhenThresholdReached, uint minimumFingers, uint maximumFingers, QRegularExpression windowRegex, uint threshold)
    : Gesture(device, triggerWhenThresholdReached, minimumFingers, maximumFingers, std::move(windowRegex)),
      m_threshold(threshold)
{
    connect(&m_timer, &QTimer::timeout, this, [this]() {
        triggered();
        Q_EMIT triggeredByTimer();
    });
    m_timer.setSingleShot(true);
}

void HoldGesture::cancelled()
{
    m_timer.stop();
    Gesture::cancelled();
}

void HoldGesture::started()
{
    if (m_triggerWhenThresholdReached)
        m_timer.start(m_threshold);

    Gesture::started();
}

void HoldGesture::triggered()
{
    m_timer.stop();
    Gesture::triggered();
}

bool HoldGesture::thresholdReached(const std::chrono::milliseconds &time) const
{
    return time >= std::chrono::milliseconds(m_threshold);
}