#include "gesture.h"

Gesture::Gesture(InputDevice device, bool triggerAfterReachingThreshold, uint minimumFingers, uint maximumFingers, QRegularExpression windowRegex)
    : device(device), triggerAfterReachingThreshold(triggerAfterReachingThreshold), minimumFingers(minimumFingers), maximumFingers(maximumFingers), windowRegex(std::move(windowRegex))
{
}

void Gesture::cancelled()
{
    for (const auto &action : cancelledActions)
        action->execute();
}

void Gesture::started()
{
    for (const auto &action : startedActions)
        action->execute();
}

void Gesture::triggered()
{
    for (const auto &action : triggerActions)
        action->execute();
}