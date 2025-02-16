#include "animation.h"

#include <QRectF>
#include <QDebug>

template class Animation<qreal>;
template class Animation<QRectF>;

template<typename T>
Animation<T>::Animation(const T &from, const T &to, const QEasingCurve &curve, const qreal &duration, const bool &keepLastFrame)
    : m_from(from)
    , m_to(to)
    , m_curve(curve)
    , m_duration(duration)
    , m_keepLastFrame(keepLastFrame)
    , m_valid(true)
{
}

template<typename T>
bool Animation<T>::isActive(const std::chrono::milliseconds &presentTime) const
{
    return m_valid && m_start.count() != 0 && (m_keepLastFrame || m_manualProgress || m_start.count() + m_duration >= presentTime.count());
}

template<typename T>
bool Animation<T>::isCompleted(const std::chrono::milliseconds &presentTime) const
{
    return isActive(presentTime) && qFuzzyCompare(m_manualProgress ? *m_manualProgress : progress(presentTime), 1.0);
}

template<typename T>
T Animation<T>::value(const qreal &progressRaw) const
{
    return valueRawProgress(progress(progressRaw));
}

template<typename T>
T Animation<T>::value(const std::chrono::milliseconds &presentTime) const
{
    return valueRawProgress(progress(presentTime));
}

template<>
qreal Animation<qreal>::valueRawProgress(const qreal &progressRaw) const
{
    return m_from + (m_to - m_from) * progressRaw;
}

template<>
QRectF Animation<QRectF>::valueRawProgress(const qreal &progressRaw) const
{
    const auto x = m_from.x() + (m_to.x() - m_from.x()) * progressRaw;
    const auto y = m_from.y() + (m_to.y() - m_from.y()) * progressRaw;
    const auto width = m_from.width() + (m_to.width() - m_from.width()) * progressRaw;
    const auto height = m_from.height() + (m_to.height() - m_from.height()) * progressRaw;
    return QRectF(x, y, width, height);
}

template<typename T>
qreal Animation<T>::progress(const std::chrono::milliseconds &presentTime) const
{
    return progress(std::clamp((presentTime.count() - m_start.count()) / m_duration, 0.0, 1.0));
}

template<typename T>
qreal Animation<T>::progress(const qreal &progressRaw) const
{
    qreal p = m_manualProgress ? *m_manualProgress : progressRaw;
//    if (!qFuzzyCompare(m_curveStart, 0)) {
//        p = m_curveStart + (1 - m_curveStart) * p;
//    }
    return m_curve.valueForProgress(p);
}

template<typename T>
void Animation<T>::start()
{
    m_start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch());
}

template<typename T>
void Animation<T>::stop()
{
    m_start = std::chrono::milliseconds(0);
}

template<typename T>
void Animation<T>::setManual(const bool &manual)
{
    if (!manual) {
        m_manualProgress = std::nullopt;
        return;
    }

    m_manualProgress = 0;
}

template<typename T>
void Animation<T>::setManualProgress(const qreal &progress)
{
    m_manualProgress = progress;
}

template<typename T>
T Animation<T>::from() const
{
    return m_from;
}

template<typename T>
T Animation<T>::to() const
{
    return m_to;
}