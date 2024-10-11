#include "config.h"

#include <KConfig>


void Config::read()
{
    gestures.clear();

    const KConfig config("kwingesturesrc", KConfig::SimpleConfig);
    const auto gesturesGroup = config.group("Gestures");
    for (const auto &deviceGroupName : gesturesGroup.groupList())
    {
        const auto deviceGroup = gesturesGroup.group(deviceGroupName);
        InputDevice device = InputDevice::Touchpad;

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
            const bool triggerAfterReachingThreshold = gestureGroup.readEntry("TriggerAfterReachingThreshold", false);
            const QRegularExpression windowRegex(gestureGroup.readEntry("WindowRegex", ""));

            std::shared_ptr<Gesture> gesture;
            if (gestureType == "Hold")
            {
                // TODO
            }
            else if (gestureType == "Pinch")
            {
                const auto pinchGestureGroup = gestureGroup.group("Pinch");

                const auto direction = pinchGestureGroup.readEntry("Direction", "Contracting") == "Contracting"
                   ? KWin::PinchDirection::Contracting
                   : KWin::PinchDirection::Expanding;
                const qreal threshold = pinchGestureGroup.readEntry("Threshold", 1.0);

                gesture = std::make_shared<PinchGesture>(device, triggerAfterReachingThreshold, minimumFingers, maximumFingers, windowRegex, direction, threshold);
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

                gesture = std::make_shared<SwipeGesture>(device, triggerAfterReachingThreshold, minimumFingers, maximumFingers, windowRegex, direction, threshold);
            }

            if (!gesture)
                continue;

            const auto actionsGroup = gestureGroup.group("Actions");
            for (const auto &actionGroupName : actionsGroup.groupList())
            {
                const auto actionGroup = actionsGroup.group(actionGroupName);
                const auto actionType = actionGroup.readEntry("Type");
//                const auto actionWhen = actionGroup.readEntry("When");

                std::unique_ptr<GestureAction> action;
                if (actionType == "Command")
                {
                    const auto commandActionGroup = actionGroup.group("Command");
                    const auto command = commandActionGroup.readEntry("Command", "");
                    action = std::make_unique<CommandGestureAction>(command);
                }
                else if (actionType == "GlobalShortcut")
                {
                    const auto globalShortcutActionGroup = actionGroup.group("GlobalShortcut");
                    const auto component = globalShortcutActionGroup.readEntry("Component", "");
                    const auto shortcut = globalShortcutActionGroup.readEntry("Shortcut", "");
                    action = std::make_unique<GlobalShortcutGestureAction>(component, shortcut);
                }

                if (action)
                    gesture->triggerActions.push_back(std::move(action));
            }

            gestures.push_back(gesture);
        }
    }
}