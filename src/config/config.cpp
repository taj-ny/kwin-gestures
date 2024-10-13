#include "config.h"
#include "effect/globals.h"
#include "gestures/actions/command.h"
#include "gestures/actions/globalshortcut.h"
#include "gestures/actions/keysequence.h"
#include "gestures/holdgesture.h"
#include "gestures/pinchgesture.h"
#include "gestures/swipegesture.h"
#include <KConfig>

void Config::read()
{
    gestures.clear();

    const KConfig config("kwingesturesrc", KConfig::SimpleConfig);
    const auto gesturesGroup = config.group("Gestures");
    for (const auto &deviceGroupName : gesturesGroup.groupList())
    {
        const auto deviceGroup = gesturesGroup.group(deviceGroupName);
        InputDeviceType device = InputDeviceType::Touchpad;

        for (const auto &gestureGroupName : deviceGroup.groupList())
        {
            const auto gestureGroup = deviceGroup.group(gestureGroupName);

            const auto gestureType = gestureGroup.readEntry("Type", "");
            const int fingers = gestureGroup.readEntry("Fingers", -1);
            uint minimumFingers;
            uint maximumFingers;
            if (fingers == -1)
            {
                minimumFingers = gestureGroup.readEntry("MinimumFingers", 2);
                maximumFingers = gestureGroup.readEntry("MaximumFingers", 2);
            }
            else
            {
                minimumFingers = maximumFingers = fingers;
            }
            const bool triggerWhenThresholdReached = gestureGroup.readEntry("TriggerWhenThresholdReached", false);

            std::shared_ptr<Gesture> gesture;
            if (gestureType == "Hold")
            {
                const auto holdGestureGroup = gestureGroup.group("Hold");

                const auto threshold = holdGestureGroup.readEntry("Threshold", 0);

                gesture = std::make_shared<HoldGesture>(device, triggerWhenThresholdReached, minimumFingers, maximumFingers, threshold);
            }
            else if (gestureType == "Pinch")
            {
                const auto pinchGestureGroup = gestureGroup.group("Pinch");

                const auto direction = pinchGestureGroup.readEntry("Direction", "Contracting") == "Contracting"
                   ? KWin::PinchDirection::Contracting
                   : KWin::PinchDirection::Expanding;
                const qreal threshold = pinchGestureGroup.readEntry("Threshold", 1.0);

                gesture = std::make_shared<PinchGesture>(device, triggerWhenThresholdReached, minimumFingers, maximumFingers, direction, threshold);
            }
            else if (gestureType == "Swipe")
            {
                const auto swipeGesture = gestureGroup.group("Swipe");

                const auto directionString = swipeGesture.readEntry("Direction", "Left");
                auto direction = KWin::SwipeDirection::Left;
                if (directionString == "Right")
                    direction = KWin::SwipeDirection::Right;
                else if (directionString == "Up")
                    direction = KWin::SwipeDirection::Up;
                else if (directionString == "Down")
                    direction = KWin::SwipeDirection::Down;

                const qreal thresholdX = swipeGesture.readEntry("ThresholdX", 0.0);
                const qreal thresholdY = swipeGesture.readEntry("ThresholdY", 0.0);
                const QPointF threshold(thresholdX, thresholdY);

                gesture = std::make_shared<SwipeGesture>(device, triggerWhenThresholdReached, minimumFingers, maximumFingers, direction, threshold);
            }

            if (!gesture)
                continue;

            const auto actionsGroup = gestureGroup.group("Actions");
            for (const auto &actionGroupName : actionsGroup.groupList())
            {
                const auto actionGroup = actionsGroup.group(actionGroupName);
                const auto actionType = actionGroup.readEntry("Type");

                std::shared_ptr<GestureAction> action;
                if (actionType == "Command")
                {
                    const auto commandActionGroup = actionGroup.group("Command");
                    const auto command = commandActionGroup.readEntry("Command", "");
                    action = std::make_shared<CommandGestureAction>(command);
                }
                else if (actionType == "GlobalShortcut")
                {
                    const auto globalShortcutActionGroup = actionGroup.group("GlobalShortcut");
                    const auto component = globalShortcutActionGroup.readEntry("Component", "");
                    const auto shortcut = globalShortcutActionGroup.readEntry("Shortcut", "");
                    action = std::make_shared<GlobalShortcutGestureAction>(component, shortcut);
                }
                else if (actionType == "KeySequence")
                {
                    const auto keySequenceActionGroup = actionGroup.group("KeySequence");
                    const auto script = keySequenceActionGroup.readEntry("Sequence", "");
                    action = std::make_shared<KeySequenceGestureAction>(script);
                }

                if (!action)
                    continue;

                const auto &conditions = readConditions(actionGroup.group("Conditions"));
                for (const auto &condition : conditions)
                    action->addCondition(condition);

                gesture->addTriggerAction(action);
            }

            const auto &conditions = readConditions(gestureGroup.group("Conditions"));
            for (const auto &condition : conditions)
                gesture->addCondition(condition);

            gestures.push_back(gesture);
        }
    }
}

std::vector<Condition> Config::readConditions(const KConfigGroup &group) const
{
    std::vector<Condition> conditions;
    for (const auto &conditionGroupName : group.groupList())
    {
        const auto conditionGroup = group.group(conditionGroupName);

        const bool negate = conditionGroup.readEntry("Negate", false);

        std::optional<QRegularExpression> windowClassRegex;
        const auto windowClassRegexStr = conditionGroup.readEntry("WindowRegex", "");
        if (windowClassRegexStr != "")
            windowClassRegex = QRegularExpression(windowClassRegexStr);

        auto windowState = WindowState::Unimportant;
        std::optional<WindowState> windowStateOpt;
        const auto windowStateStr = conditionGroup.readEntry("WindowState", "");
        for (const auto &windowStateFlag : windowStateStr.split("|"))
        {
            if (windowStateFlag == "Fullscreen")
                windowState = windowState | WindowState::Fullscreen;
            else if (windowStateFlag == "Maximized")
                windowState = windowState | WindowState::Maximized;
        }
        if (windowState)
            windowStateOpt = windowState;

        conditions.emplace_back(negate, windowClassRegex, windowStateOpt);
    }

    return conditions;
}