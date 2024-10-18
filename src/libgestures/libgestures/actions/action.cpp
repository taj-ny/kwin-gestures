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

void GestureAction::execute()
{
    m_triggered = true;
    Q_EMIT executed();
}

bool GestureAction::canExecute() const
{
    return m_repeatInterval != 0 || !m_triggered;
}

bool GestureAction::blocksOtherActions() const
{
    return m_triggered && m_blockOtherActions;
}

void GestureAction::onGestureCancelled()
{
    if (m_when == When::Cancelled && canExecute() && satisfiesConditions())
        execute();

    m_accumulatedDelta = 0;
    m_triggered = false;
}

void GestureAction::onGestureEnded()
{
    if (m_when == When::Ended && canExecute() && satisfiesConditions())
        execute();

    m_accumulatedDelta = 0;
    m_triggered = false;
}

void GestureAction::onGestureStarted()
{
    if (m_when == When::Started && canExecute() && satisfiesConditions())
        execute();

    m_triggered = false;
    m_accumulatedDelta = 0;
}

void GestureAction::onGestureUpdated(const qreal &delta)
{
    if (m_repeatInterval == 0 || m_when != When::Updated)
        return;

    if ((m_accumulatedDelta > 0 && delta < 0) || (m_accumulatedDelta < 0 && delta > 0))
    {
        // Direction changed, reset delta
        m_accumulatedDelta = 0;
    }
    else
        m_accumulatedDelta += delta;

    while (((m_accumulatedDelta > 0 && m_repeatInterval > 0) || (m_accumulatedDelta < 0 && m_repeatInterval < 0)) && std::abs(m_accumulatedDelta / m_repeatInterval) >= 1)
    {
        execute();
        m_accumulatedDelta -= m_repeatInterval;
    }
}

void GestureAction::setBlockOtherActions(const bool &blockOtherActions)
{
    m_blockOtherActions = blockOtherActions;
}

void GestureAction::setRepeatInterval(const qreal &interval)
{
    m_repeatInterval = interval;
}

} // namespace libgestures