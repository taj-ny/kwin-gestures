#include "action.h"

namespace libgestures
{

GestureAction::GestureAction()
{
    connect(this, &GestureAction::gestureCancelled, this, &GestureAction::onGestureCancelled);
    connect(this, &GestureAction::gestureEnded, this, &GestureAction::onGestureEnded);
    connect(this, &GestureAction::gestureStarted, this, &GestureAction::onGestureStarted);
    connect(this, &GestureAction::gestureUpdated, this, &GestureAction::onGestureUpdated);
}

void GestureAction::addCondition(const std::shared_ptr<const Condition> &condition)
{
    m_conditions.push_back(condition);
}

bool GestureAction::satisfiesConditions() const
{
    return m_conditions.empty() || std::find_if(m_conditions.begin(), m_conditions.end(), [](const std::shared_ptr<const Condition> &condition)
    {
        return condition->isSatisfied();
    }) != m_conditions.end();
}

bool GestureAction::thresholdReached() const
{
    return (m_minimumThreshold == 0 || m_absoluteAccumulatedDelta >= m_minimumThreshold)
        && (m_maximumThreshold == 0 || m_absoluteAccumulatedDelta <= m_maximumThreshold);
}

bool GestureAction::tryExecute()
{
    if (!satisfiesConditions() || !thresholdReached()) {
        return false;
    }

    m_executed = true;
    Q_EMIT executed();
    return true;
}

bool GestureAction::blocksOtherActions() const
{
    return m_executed && m_blockOtherActions;
}

void GestureAction::onGestureCancelled()
{
    if (m_on == On::Cancel || m_on == On::EndOrCancel) {
        tryExecute();
    }

    m_accumulatedDelta = 0;
    m_absoluteAccumulatedDelta = 0;
}

void GestureAction::onGestureEnded()
{
    if (m_on == On::End || m_on == On::EndOrCancel) {
        tryExecute();
    }

    m_accumulatedDelta = 0;
    m_absoluteAccumulatedDelta = 0;
}

void GestureAction::onGestureStarted()
{
    m_executed = false;
    if (m_on == On::Begin) {
        tryExecute();
    }

    m_accumulatedDelta = 0;
    m_absoluteAccumulatedDelta = 0;
}

void GestureAction::onGestureUpdated(const qreal &delta, const QPointF &deltaPointMultiplied)
{
    m_currentDeltaPointMultiplied = deltaPointMultiplied;
    if ((m_accumulatedDelta > 0 && delta < 0) || (m_accumulatedDelta < 0 && delta > 0)) {
        // Direction changed, reset delta
        m_accumulatedDelta = 0;
    } else {
        m_accumulatedDelta += delta;
        m_absoluteAccumulatedDelta += std::abs(delta);
    }

    if (m_on != On::Update) {
        return;
    }

    if (m_repeatInterval != 0) {
        while (((m_accumulatedDelta > 0 && m_repeatInterval > 0) || (m_accumulatedDelta < 0 && m_repeatInterval < 0))
            && std::abs(m_accumulatedDelta / m_repeatInterval) >= 1) {
            tryExecute();
            m_accumulatedDelta -= m_repeatInterval;
        }
        return;
    }

    tryExecute();
}

void GestureAction::setBlockOtherActions(const bool &blockOtherActions)
{
    m_blockOtherActions = blockOtherActions;
}

void GestureAction::setRepeatInterval(const qreal &interval)
{
    m_repeatInterval = interval;
}

void GestureAction::setThresholds(const qreal &minimum, const qreal &maximum)
{
    m_minimumThreshold = minimum;
    m_maximumThreshold = maximum;
}

void GestureAction::setOn(const libgestures::On &on)
{
    m_on = on;
}

}