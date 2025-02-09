#include "callback.h"

namespace libgestures
{

CallbackCondition::CallbackCondition(const std::function<bool()> &callback)
    : m_callback(callback)
{
}

bool CallbackCondition::isSatisfied() const
{
    return m_callback();
}

}