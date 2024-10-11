#pragma once

#include <QObject>
#include "effect/globals.h"
#include "options.h"
#include <QPointF>

enum InputDevice
{
    Touchpad
};

class GestureAction
{
public:
    virtual void execute() { };

protected:
    GestureAction() = default;
};

class GlobalShortcutGestureAction : public GestureAction
{
public:
    GlobalShortcutGestureAction(QString component, QString shortcut);

    QString component;
    QString shortcut;

    void execute() override;
};

class KeySequenceGestureAction : public GestureAction
{

    void execute() override;
};

class Gesture
{
public:
    virtual ~Gesture() = default;

    InputDevice device;
    bool triggerAfterReachingThreshold;
    uint minimumFingers;
    uint maximumFingers;

    std::vector<std::unique_ptr<GestureAction>> cancelledActions;
    std::vector<std::unique_ptr<GestureAction>> startedActions;
    std::vector<std::unique_ptr<GestureAction>> triggerActions;

    void cancelled();
    void started();
    void triggered();

protected:
    Gesture(InputDevice device, bool triggerAfterReachingThreshold, uint minimumFingers, uint maximumFingers);
};

class HoldGesture : public Gesture
{
    uint threshold;

    bool thresholdReached(uint time) const;
};

class SwipeGesture : public Gesture
{
public:
    SwipeGesture(InputDevice device, bool triggerAfterReachingThreshold, uint minimumFingers, uint maximumFingers, KWin::SwipeDirection direction, QPointF threshold);

    KWin::SwipeDirection direction;
    QPointF threshold;

    bool thresholdReached(const QPointF &delta) const;
};

class PinchGesture : public Gesture
{
public:
    PinchGesture(InputDevice device, bool triggerAfterReachingThreshold, uint minimumFingers, uint maximumFingers, KWin::PinchDirection direction, qreal threshold);

    KWin::PinchDirection direction;
    qreal threshold;

    bool thresholdReached(const qreal &scale) const;
};