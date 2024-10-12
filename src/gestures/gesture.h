#pragma once

#include "actions/action.h"
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

    virtual void cancelled();
    virtual void started();
    virtual void triggered();

    bool meetsConditions(uint fingerCount) const;

    void addTriggerAction(std::unique_ptr<GestureAction> action);
protected:
    Gesture(InputDeviceType device, bool triggerWhenThresholdReached, uint minimumFingers, uint maximumFingers, QRegularExpression windowRegex);

    const bool m_triggerWhenThresholdReached;
private:
    const InputDeviceType m_device;
    const uint m_minimumFingers;
    const uint m_maximumFingers;
    const QRegularExpression m_windowRegex;

    std::vector<std::unique_ptr<GestureAction>> m_cancelledActions;
    std::vector<std::unique_ptr<GestureAction>> m_startedActions;
    std::vector<std::unique_ptr<GestureAction>> m_triggerActions;
};