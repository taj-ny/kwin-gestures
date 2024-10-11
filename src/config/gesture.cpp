#include "gesture.h"
#include <QDBusInterface>
#include <utility>

CommandGestureAction::CommandGestureAction(QString command)
    : m_command(std::move(command))
{
}

void CommandGestureAction::execute()
{
    std::system((m_command + " &").toStdString().c_str());
}

GlobalShortcutGestureAction::GlobalShortcutGestureAction(QString component, QString shortcut)
    : component(std::move(component)), shortcut(std::move(shortcut))
{
}

void GlobalShortcutGestureAction::execute()
{
    QDBusInterface interface("org.kde.kglobalaccel", "/component/" + component, "org.kde.kglobalaccel.Component");
    interface.call("invokeShortcut", shortcut);
}

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

SwipeGesture::SwipeGesture(InputDevice device, bool triggerAfterReachingThreshold, uint minimumFingers, uint maximumFingers, QRegularExpression windowRegex, KWin::SwipeDirection direction, QPointF threshold)
    : Gesture(device, triggerAfterReachingThreshold, minimumFingers, maximumFingers, windowRegex), direction(direction), threshold(threshold)
{
}

bool SwipeGesture::thresholdReached(const QPointF &delta) const
{
    switch (direction)
    {
        case KWin::SwipeDirection::Up:
        case KWin::SwipeDirection::Down:
            return std::abs(delta.y()) >= std::abs(threshold.y());
        case KWin::SwipeDirection::Left:
        case KWin::SwipeDirection::Right:
            return std::abs(delta.x()) >= std::abs(threshold.x());
        default:
            Q_UNREACHABLE();
    }
}

PinchGesture::PinchGesture(InputDevice device, bool triggerAfterReachingThreshold, uint minimumFingers, uint maximumFingers, QRegularExpression windowRegex, KWin::PinchDirection direction, qreal threshold)
    : Gesture(device, triggerAfterReachingThreshold, minimumFingers, maximumFingers, windowRegex), direction(direction), threshold(threshold)
{
}

bool PinchGesture::thresholdReached(const qreal &scale) const
{
    return direction == KWin::PinchDirection::Expanding
        ? scale >= threshold
        : scale <= threshold;
}