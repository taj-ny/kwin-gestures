#include "conditiongroup.h"

#include <algorithm>

namespace libgestures
{

bool ConditionGroup::satisfied() const
{
    const auto begin = m_conditions.begin();
    const auto end = m_conditions.end();
    const auto pred = [](const auto &condition) {
        return condition->satisfied();
    };
    switch (m_mode)
    {
        case ConditionGroupMode::All:
            return std::all_of(begin, end, pred);
        case ConditionGroupMode::Any:
            return std::any_of(begin, end, pred);
        case ConditionGroupMode::None:
            return std::none_of(begin, end, pred);
        default:
            return false;
    }
}

void ConditionGroup::add(const std::shared_ptr<ConditionV2> &condition)
{
    m_conditions.push_back(condition);
}

void ConditionGroup::setMode(const ConditionGroupMode &mode)
{
    m_mode = mode;
}

}