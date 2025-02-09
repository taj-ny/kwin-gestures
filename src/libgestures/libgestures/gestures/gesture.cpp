#include "gesture.h"

namespace libgestures
{

Gesture::Gesture()
{
    connect(this, &Gesture::cancelled, this, &Gesture::onCancelled);
    connect(this, &Gesture::ended, this, &Gesture::onEnded);
    connect(this, &Gesture::started, this, &Gesture::onStarted);
    connect(this, &Gesture::updated, this, &Gesture::onUpdated);
}

void Gesture::onCancelled()
{
    m_absoluteAccumulatedDelta = 0;
    m_accumulatedDelta = 0;

    if (!m_hasStarted)
        return;

    m_hasStarted = false;

    for (const auto &action : m_actions) {
        Q_EMIT action->gestureCancelled();
        if (action->blocksOtherActions())
            break;
    }
}

void Gesture::onEnded()
{
    const auto oldDelta = m_accumulatedDelta;
    m_absoluteAccumulatedDelta = 0;
    m_accumulatedDelta = 0;

    if (!m_hasStarted)
        return;

    if ((m_minimumDelta != 0) && oldDelta < m_minimumDelta) {
        Q_EMIT cancelled();
        return;
    }
    m_hasStarted = false;

    for (const auto &action : m_actions) {
        Q_EMIT action->gestureEnded();
        if (action->blocksOtherActions())
            break;
    }
}

void Gesture::onStarted()
{
    if (!m_hasStarted)
        return;

    for (const auto &action : m_actions) {
        Q_EMIT action->gestureStarted();
        if (action->blocksOtherActions())
            break;
    }
}

void Gesture::onUpdated(const QPointF &delta, const QPointF &deltaPointMultiplied, bool &endedPrematurely)
{
    const auto realDelta = getRealDelta(delta);
    m_absoluteAccumulatedDelta += std::abs(realDelta);
    m_accumulatedDelta += realDelta;
    if (!thresholdReached())
        return;

    if (!m_hasStarted) {
        m_hasStarted = true;
        Q_EMIT started();
    }

    if (m_maximumDelta != 0 && (m_accumulatedDelta - m_minimumThreshold >= m_maximumDelta)) {
        endedPrematurely = true;
        Q_EMIT ended();
        return;
    }

    const auto progress = m_maximumDelta == 0
        ? 1.0
        : std::clamp((m_accumulatedDelta - m_minimumThreshold) / m_maximumDelta, 0.0, 1.0);
    for (const auto &action : m_actions) {
        Q_EMIT action->gestureUpdated(realDelta, progress, deltaPointMultiplied);
        if (action->blocksOtherActions()) {
            endedPrematurely = true;
            Q_EMIT ended();
            return;
        }
    }
}

bool Gesture::satisfiesConditions(const uint8_t &fingerCount) const
{
    if (m_minimumFingers > fingerCount || m_maximumFingers < fingerCount) {
        return false;
    }

    if (!m_conditions.empty()) {
        bool satisfies = false;
        for (const auto &condition: m_conditions) {
            const bool satisfied = condition->isSatisfied();
            if (condition->required() && !satisfied) {
                return false;
            }

            satisfies |= satisfied;
        }

        if (!satisfies) {
            return false;
        }
    }

    const bool actionSatisfiesConditions = std::find_if(m_actions.begin(), m_actions.end(), [](const std::unique_ptr<GestureAction> &triggerAction) {
        return triggerAction->satisfiesConditions();
    }) != m_actions.end();
    return m_actions.empty() || actionSatisfiesConditions;
}

void Gesture::addAction(std::unique_ptr<GestureAction> action)
{
    m_actions.push_back(std::move(action));
}

void Gesture::addCondition(const std::shared_ptr<const Condition> &condition)
{
    m_conditions.push_back(condition);
}

void Gesture::setThresholds(const qreal &minimum, const qreal &maximum)
{
    m_minimumThreshold = minimum;
    m_maximumThreshold = maximum;
}

void Gesture::setFingers(const uint8_t &minimum, const uint8_t &maximum)
{
    m_minimumFingers = minimum;
    m_maximumFingers = maximum;
}

void Gesture::setMinimumDelta(const qreal &delta)
{
    m_minimumDelta = delta;
}

void Gesture::setMaximumDelta(const qreal &delta)
{
    m_maximumDelta = delta;
}

bool Gesture::thresholdReached() const
{
    return ((m_minimumThreshold == 0 || m_absoluteAccumulatedDelta >= m_minimumThreshold)
            && (m_maximumThreshold == 0 || m_absoluteAccumulatedDelta <= m_maximumThreshold));
}

void Gesture::setSpeed(const libgestures::GestureSpeed &speed)
{
    m_speed = speed;
}

void Gesture::setBlocksOthers(const bool &blocksOthers)
{
    m_blocksOthers = blocksOthers;
}

const bool &Gesture::blocksOthers() const
{
    return m_blocksOthers;
}

GestureSpeed Gesture::speed() const
{
    return m_speed;
}

qreal Gesture::getRealDelta(const QPointF &delta) const
{
    return delta.x();
}

}