#include "action.h"

namespace libgestures
{

GestureAction::GestureAction(qreal repeatInterval)
    : m_repeatInterval(repeatInterval)
{
}

void GestureAction::addCondition(const Condition &condition)
{
    m_conditions.push_back(condition);
}

bool GestureAction::satisfiesConditions() const
{
    return m_conditions.empty() || std::find_if(m_conditions.begin(), m_conditions.end(), [](const Condition &condition)
    {
        return condition.isSatisfied();
    }) != m_conditions.end();
}

void GestureAction::execute()
{
    m_triggered = true;
}

bool GestureAction::canExecute() const
{
    return m_repeatInterval != 0 || !m_triggered;
}

void GestureAction::cancelled()
{
    if (m_when == When::Cancelled && canExecute() && satisfiesConditions())
        execute();

    m_accumulatedDelta = 0;
    m_triggered = false;
}

void GestureAction::ended()
{
    if (m_when == When::Ended && canExecute() && satisfiesConditions())
        execute();

    m_accumulatedDelta = 0;
    m_triggered = false;
}

void GestureAction::started()
{
    if (m_when == When::Started && canExecute() && satisfiesConditions())
        execute();

    m_triggered = false;
    m_accumulatedDelta = 0;
}

void GestureAction::update(const qreal &delta)
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

} // namespace libgestures