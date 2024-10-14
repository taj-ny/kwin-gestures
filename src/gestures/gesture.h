#pragma once

#include "actions/action.h"
#include "condition.h"
#include <QRegularExpression>
#include <vector>

enum InputDeviceType
{
    Touchpad
};

class Gesture : public QObject
{
    Q_OBJECT
public:
    virtual ~Gesture() = default;

    bool triggerWhenThresholdReached() const { return m_triggerWhenThresholdReached; };

    virtual void cancelled() { };
    virtual void started() { };
    virtual void triggered();

    bool satisfiesConditions(uint fingerCount) const;

    void addTriggerAction(const std::shared_ptr<const GestureAction> &action);
    void addCondition(const Condition &condition);
protected:
    Gesture(InputDeviceType device, bool triggerWhenThresholdReached, uint minimumFingers, uint maximumFingers, bool triggerOneActionOnly);

    const bool m_triggerWhenThresholdReached;
private:
    const InputDeviceType m_device;
    const uint m_minimumFingers;
    const uint m_maximumFingers;
    const bool m_triggerOneActionOnly;

    std::vector<Condition> m_conditions;

    std::vector<std::shared_ptr<const GestureAction>> m_triggerActions;
};