#include "gesture.h"

namespace libgestures
{

static const qreal s_inertiaTimerFrequency = 240.0; // Events per second
static const uint32_t s_inertiaEventMultiplier = 5; // Split one event into multiple to make inertia smoother

Gesture::Gesture()
{
    connect(this, &Gesture::cancelled, this, &Gesture::onCancelled);
    connect(this, &Gesture::ended, this, &Gesture::onEnded);
    connect(this, &Gesture::started, this, &Gesture::onStarted);
    connect(this, &Gesture::updated, this, &Gesture::onUpdated);

    m_inertiaTimer.setInterval(1000 / s_inertiaTimerFrequency);
    connect(&m_inertiaTimer, &QTimer::timeout, this, &Gesture::onUpdatedInertia);
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
    if (!m_hasStarted) {
        return;
    }

    if (m_inertiaTimer.isActive()) {
        m_inertiaTimer.stop();
        m_velocity = {};
        m_inertiaEvents = 0;
    } else if (m_inertial && (std::abs(m_velocity.x()) >= 20 || std::abs(m_velocity.y()) >= 20)) {
        m_inertiaTimer.start();
        return;
    }

    const auto oldDelta = m_accumulatedDelta;
    m_absoluteAccumulatedDelta = 0;
    m_accumulatedDelta = 0;

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
    m_velocity = delta;
    if (!thresholdReached())
        return;

    if (!m_hasStarted) {
        m_hasStarted = true;
        Q_EMIT started();
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

    if (m_maximumDelta != 0 && (m_accumulatedDelta - m_minimumThreshold >= m_maximumDelta)) {
        endedPrematurely = true;
        Q_EMIT ended();
        return;
    }
}

void Gesture::onUpdatedInertia()
{
    bool endedPrematurely = false;
    const auto velocity = m_velocity;
    Q_EMIT updated(m_velocity / s_inertiaEventMultiplier, QPointF(), endedPrematurely);
    m_velocity = velocity;

    if (m_inertiaEvents++ % s_inertiaEventMultiplier == 0) {
        m_velocity *= m_inertiaFriction;
    }

    if (endedPrematurely || std::abs(m_velocity.x()) + std::abs(m_velocity.y()) < 0.1) {
        Q_EMIT ended();
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

void Gesture::setInertiaFriction(const qreal &friction)
{
    m_inertiaFriction = friction;
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