#include "actions/command.h"
#include "actions/kdeglobalshortcut.h"
#include "actions/keysequence.h"
#include "config.h"
#include "gestures/holdgesture.h"
#include "gestures/pinchgesture.h"
#include "gestures/swipegesture.h"
#include <KConfig>

void Config::read(std::shared_ptr<GestureInputEventFilter> filter, std::shared_ptr<VirtualInputDevice> virtualInputDevice, std::shared_ptr<WindowDataProvider> windowDataProvider)
{
    filter->unregisterGestures();

    const KConfig config("kwingesturesrc", KConfig::SimpleConfig);
    const auto gesturesGroup = config.group("Gestures");
    for (const auto &deviceGroupName : gesturesGroup.groupList())
    {
        const auto deviceGroup = gesturesGroup.group(deviceGroupName);

        for (const auto &gestureId : stringIntListToSortedIntVector(deviceGroup.groupList()))
        {
            const auto gestureGroup = deviceGroup.group(QString::number(gestureId));

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
            const bool triggerOneActionOnly = gestureGroup.readEntry("TriggerOneActionOnly", false);

            std::shared_ptr<Gesture> gesture;
            if (gestureType == "Hold")
            {
                const auto holdGestureGroup = gestureGroup.group("Hold");

                const auto threshold = holdGestureGroup.readEntry("Threshold", 0);

                const auto holdGesture = std::make_shared<HoldGesture>(triggerWhenThresholdReached, minimumFingers, maximumFingers, triggerOneActionOnly, threshold);
                filter->registerTouchpadGesture(holdGesture);
                gesture = holdGesture;
            }
            else if (gestureType == "Pinch")
            {
                const auto pinchGestureGroup = gestureGroup.group("Pinch");

                const auto directionStr = pinchGestureGroup.readEntry("Direction", "");
                PinchDirection direction = PinchDirection::Any;
                if (directionStr == "Contracting")
                    direction = PinchDirection::Contracting;
                else if (directionStr == "Expanding")
                    direction = PinchDirection::Expanding;
                const qreal threshold = pinchGestureGroup.readEntry("Threshold", 1.0);

                const auto pinchGesture = std::make_shared<PinchGesture>(triggerWhenThresholdReached, minimumFingers, maximumFingers, triggerOneActionOnly, threshold, direction);
                filter->registerTouchpadGesture(pinchGesture);
                gesture = pinchGesture;
            }
            else if (gestureType == "Swipe")
            {
                const auto swipeGestureGroup = gestureGroup.group("Swipe");

                const auto directionString = swipeGestureGroup.readEntry("Direction", "Left");
                auto direction = SwipeDirection::Left;
                if (directionString == "Right")
                    direction = SwipeDirection::Right;
                else if (directionString == "Up")
                    direction = SwipeDirection::Up;
                else if (directionString == "Down")
                    direction = SwipeDirection::Down;
                else if (directionString == "LeftRight")
                    direction = SwipeDirection::LeftRight;
                else if (directionString == "UpDown")
                    direction = SwipeDirection::UpDown;

                const qreal threshold = swipeGestureGroup.readEntry("Threshold", 0.0);

                const auto swipeGesture = std::make_shared<SwipeGesture>(triggerWhenThresholdReached, minimumFingers, maximumFingers, triggerOneActionOnly, threshold, direction);
                filter->registerTouchpadGesture(swipeGesture);
                gesture = swipeGesture;
            }

            if (!gesture)
                continue;

            const auto actionsGroup = gestureGroup.group("Actions");
            for (const auto &actionId : stringIntListToSortedIntVector(actionsGroup.groupList()))
            {
                const auto actionGroup = actionsGroup.group(QString::number(actionId));
                const auto actionType = actionGroup.readEntry("Type", "");
                const auto repeat = actionGroup.readEntry("Repeat", false);
                const auto repeatInterval = actionGroup.readEntry("RepeatInterval", 0.0);

                std::shared_ptr<GestureAction> action;
                if (actionType == "Command")
                {
                    const auto commandActionGroup = actionGroup.group("Command");
                    const auto command = commandActionGroup.readEntry("Command", "");
                    action = std::make_shared<CommandGestureAction>(repeatInterval, command);
                }
                else if (actionType == "GlobalShortcut")
                {
                    const auto globalShortcutActionGroup = actionGroup.group("GlobalShortcut");
                    const auto component = globalShortcutActionGroup.readEntry("Component", "");
                    const auto shortcut = globalShortcutActionGroup.readEntry("Shortcut", "");
                    action = std::make_shared<KDEGlobalShortcutGestureAction>(repeatInterval, component, shortcut);
                }
                else if (actionType == "KeySequence")
                {
                    const auto keySequenceActionGroup = actionGroup.group("KeySequence");
                    const auto script = keySequenceActionGroup.readEntry("Sequence", "");
                    action = std::make_shared<KeySequenceGestureAction>(repeatInterval, virtualInputDevice, script);
                }

                if (!action)
                    continue;

                const auto &conditions = readConditions(actionGroup.group("Conditions"), windowDataProvider);
                for (const auto &condition : conditions)
                    action->addCondition(condition);

                gesture->addAction(action);
            }

            const auto &conditions = readConditions(gestureGroup.group("Conditions"), windowDataProvider);
            for (const auto &condition : conditions)
                gesture->addCondition(condition);
        }
    }
}

std::vector<Condition> Config::readConditions(const KConfigGroup &group, std::shared_ptr<WindowDataProvider> windowDataProvider)
{
    std::vector<Condition> conditions;
    for (const auto &conditionGroupName : group.groupList())
    {
        const auto conditionGroup = group.group(conditionGroupName);

        const bool negate = conditionGroup.readEntry("Negate", false);

        std::optional<QRegularExpression> windowClassRegex;
        const auto windowClassRegexStr = conditionGroup.readEntry("WindowClassRegex", "");
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

        conditions.emplace_back(windowDataProvider, negate, windowClassRegex, windowStateOpt);
    }

    return conditions;
}

std::vector<int> Config::stringIntListToSortedIntVector(const QList<QString> &list)
{
    std::vector<int> ints;
    for (const auto &x : list)
        ints.push_back(x.toInt());
    std::sort(ints.begin(), ints.end());
    return ints;
}