#include "config.h"
#include <KConfig>
#include "libgestures/actions/command.h"
#include "libgestures/actions/kdeglobalshortcut.h"
#include "libgestures/actions/keysequence.h"
#include "libgestures/gestures/pinchgesture.h"
#include "libgestures/gestures/swipegesture.h"

void Config::read(std::shared_ptr<GestureInputEventFilter> filter, std::shared_ptr<libgestures::Input> input, std::shared_ptr<libgestures::WindowInfoProvider> windowInfoProvider)
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
            uint8_t minimumFingers;
            uint8_t maximumFingers;
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

            std::shared_ptr<libgestures::Gesture> gesture;
            if (gestureType == "Hold")
            {
                const auto holdGestureGroup = gestureGroup.group("Hold");

                const auto holdGesture = std::make_shared<libgestures::HoldGesture>();
                holdGesture->setThreshold(holdGestureGroup.readEntry("Threshold", 0));
                filter->registerTouchpadGesture(holdGesture);
                gesture = holdGesture;
            }
            else if (gestureType == "Pinch")
            {
                const auto pinchGestureGroup = gestureGroup.group("Pinch");

                const auto pinchGesture = std::make_shared<libgestures::PinchGesture>();
                const auto directionStr = pinchGestureGroup.readEntry("Direction", "");
                libgestures::PinchDirection direction = libgestures::PinchDirection::Any;
                if (directionStr == "Contracting")
                    direction = libgestures::PinchDirection::Contracting;
                else if (directionStr == "Expanding")
                    direction = libgestures::PinchDirection::Expanding;
                pinchGesture->setDirection(direction);
                pinchGesture->setThreshold(pinchGestureGroup.readEntry("Threshold", 1.0));

                filter->registerTouchpadGesture(pinchGesture);
                gesture = pinchGesture;
            }
            else if (gestureType == "Swipe")
            {
                const auto swipeGestureGroup = gestureGroup.group("Swipe");

                const auto swipeGesture = std::make_shared<libgestures::SwipeGesture>();
                const auto directionString = swipeGestureGroup.readEntry("Direction", "Left");
                auto direction = libgestures::SwipeDirection::Left;
                if (directionString == "Right")
                    direction = libgestures::SwipeDirection::Right;
                else if (directionString == "Up")
                    direction = libgestures::SwipeDirection::Up;
                else if (directionString == "Down")
                    direction = libgestures::SwipeDirection::Down;
                else if (directionString == "LeftRight")
                    direction = libgestures::SwipeDirection::LeftRight;
                else if (directionString == "UpDown")
                    direction = libgestures::SwipeDirection::UpDown;
                swipeGesture->setDirection(direction);
                swipeGesture->setThreshold(swipeGestureGroup.readEntry("Threshold", 0.0));

                filter->registerTouchpadGesture(swipeGesture);
                gesture = swipeGesture;
            }

            gesture->setFingers(minimumFingers, maximumFingers);
            gesture->setTriggerWhenThresholdReached(triggerWhenThresholdReached);
            gesture->setTriggerOneActionOnly(triggerOneActionOnly);

            if (!gesture)
                continue;

            const auto actionsGroup = gestureGroup.group("Actions");
            for (const auto &actionId : stringIntListToSortedIntVector(actionsGroup.groupList()))
            {
                const auto actionGroup = actionsGroup.group(QString::number(actionId));
                const auto actionType = actionGroup.readEntry("Type", "");

                std::shared_ptr<libgestures::GestureAction> action;
                if (actionType == "Command")
                {
                    const auto commandActionGroup = actionGroup.group("Command");
                    auto commandAction = std::make_shared<libgestures::CommandGestureAction>();
                    commandAction->setCommand(commandActionGroup.readEntry("Command", ""));
                    action = commandAction;
                }
                else if (actionType == "GlobalShortcut")
                {
                    const auto globalShortcutActionGroup = actionGroup.group("GlobalShortcut");
                    auto kgeGlobalShortcutAction = std::make_shared<libgestures::KDEGlobalShortcutGestureAction>();
                    kgeGlobalShortcutAction->setComponent(globalShortcutActionGroup.readEntry("Component", ""));
                    kgeGlobalShortcutAction->setShortcut(globalShortcutActionGroup.readEntry("Shortcut", ""));
                    action = kgeGlobalShortcutAction;
                }
                else if (actionType == "KeySequence")
                {
                    const auto keySequenceActionGroup = actionGroup.group("KeySequence");
                    auto keySequenceAction = std::make_shared<libgestures::KeySequenceGestureAction>(input);
                    keySequenceAction->setSequence(keySequenceActionGroup.readEntry("Sequence", ""));
                    action = keySequenceAction;
                }

                if (!action)
                    continue;

                action->setRepeatInterval(actionGroup.readEntry("RepeatInterval", 0.0));

                const auto conditions = readConditions(actionGroup.group("Conditions"), windowInfoProvider);
                for (const auto &condition : conditions)
                    action->addCondition(condition);

                gesture->addAction(action);
            }

            const auto conditions = readConditions(gestureGroup.group("Conditions"), windowInfoProvider);
            for (const auto &condition : conditions)
                gesture->addCondition(condition);
        }
    }
}

std::vector<std::shared_ptr<libgestures::Condition>> Config::readConditions(const KConfigGroup &group, std::shared_ptr<libgestures::WindowInfoProvider> windowInfoProvider)
{
    std::vector<std::shared_ptr<libgestures::Condition>> conditions;
    for (const auto &conditionGroupName : group.groupList())
    {
        const auto conditionGroup = group.group(conditionGroupName);

        auto condition = std::make_shared<libgestures::Condition>(windowInfoProvider);
        condition->setNegate(conditionGroup.readEntry("Negate", false));

        const auto windowClassRegex = conditionGroup.readEntry("WindowClassRegex", "");
        if (windowClassRegex != "")
            condition->setWindowClassRegex(QRegularExpression(windowClassRegex));

        auto windowState = libgestures::WindowState::Unimportant;
        std::optional<libgestures::WindowState> windowStateOpt;
        const auto windowStateStr = conditionGroup.readEntry("WindowState", "");
        for (const auto &windowStateFlag : windowStateStr.split("|"))
        {
            if (windowStateFlag == "Fullscreen")
                windowState = windowState | libgestures::WindowState::Fullscreen;
            else if (windowStateFlag == "Maximized")
                windowState = windowState | libgestures::WindowState::Maximized;
        }
        if (windowState)
            condition->setWindowState(windowState);

        conditions.push_back(condition);
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