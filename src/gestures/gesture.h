#pragma once

#include "actions/action.h"
#include <QRegularExpression>
#include <vector>

enum InputDevice
{
    Touchpad
};

class Gesture
{
public:
    virtual ~Gesture() = default;

    InputDevice device;
    bool triggerAfterReachingThreshold;
    uint minimumFingers;
    uint maximumFingers;
    QRegularExpression windowRegex;

    std::vector<std::unique_ptr<GestureAction>> cancelledActions;
    std::vector<std::unique_ptr<GestureAction>> startedActions;
    std::vector<std::unique_ptr<GestureAction>> triggerActions;

    void cancelled();
    void started();
    void triggered();

protected:
    Gesture(InputDevice device, bool triggerAfterReachingThreshold, uint minimumFingers, uint maximumFingers, QRegularExpression windowRegex);
};