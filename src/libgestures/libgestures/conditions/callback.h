#pragma once

#include "condition.h"

namespace libgestures
{

class CallbackCondition : public Condition
{
public:
    CallbackCondition(const std::function<bool()> &callback);
    bool isSatisfied() const override;

private:
    std::function<bool()> m_callback;
};

}