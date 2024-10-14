#include "action.h"

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