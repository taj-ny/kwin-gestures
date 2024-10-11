#pragma once

#include <QObject>
#include "effect/globals.h"
#include "options.h"
#include <QPointF>
#include <QRegularExpression>

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

class CommandGestureAction : public GestureAction
{
public:
    CommandGestureAction(QString command);

    void execute() override;
private:
    const QString m_command;
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

class HoldGesture : public Gesture
{
public:

};

class SwipeGesture : public Gesture
{
public:
    SwipeGesture(InputDevice device, bool triggerAfterReachingThreshold, uint minimumFingers, uint maximumFingers, QRegularExpression windowRegex, KWin::SwipeDirection direction, QPointF threshold);

    KWin::SwipeDirection direction;
    QPointF threshold;

    bool thresholdReached(const QPointF &delta) const;
};

class PinchGesture : public Gesture
{
public:
    PinchGesture(InputDevice device, bool triggerAfterReachingThreshold, uint minimumFingers, uint maximumFingers, QRegularExpression windowRegex, KWin::PinchDirection direction, qreal threshold);

    KWin::PinchDirection direction;
    qreal threshold;

    bool thresholdReached(const qreal &scale) const;
};