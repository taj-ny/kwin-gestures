#pragma once

#include "gestures/condition.h"

class GestureAction
{
public:
    virtual ~GestureAction() = default;

    virtual void execute() const { };

    void addCondition(const Condition &condition);
    bool satisfiesConditions() const;
protected:
    GestureAction() = default;
private:
    std::vector<Condition> m_conditions;
};