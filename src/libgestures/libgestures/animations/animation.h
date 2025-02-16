#pragma once

#include <chrono>

#include <QEasingCurve>

template<typename T>
class Animation
{
public:
    Animation() = default;
    Animation(const T &from, const T &to, const QEasingCurve &curve, const qreal &duration, const bool &keepLastFrame = true);

    bool isActive(const std::chrono::milliseconds &presentTime) const;
    bool isCompleted(const std::chrono::milliseconds &presentTime) const;

    qreal progress(const std::chrono::milliseconds &presentTime) const;
    qreal progress(const qreal &progressRaw) const;

    T value(const qreal &progress) const;
    T value(const std::chrono::milliseconds &presentTime) const;

    T from() const;
    T to() const;

    void setManual(const bool &manual);
    void setManualProgress(const qreal &progress);

    void start();
    void stop();

private:
    T valueRawProgress(const qreal &progressRaw) const;

    bool m_valid = false;
    std::chrono::milliseconds m_start;
    std::optional<qreal> m_manualProgress;
    T m_from;
    T m_to;
    QEasingCurve m_curve;
    qreal m_duration;
    bool m_keepLastFrame;
};