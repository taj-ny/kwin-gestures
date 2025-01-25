#pragma once

#include "libgestures/actions/command.h"
#include "libgestures/actions/kdeglobalshortcut.h"
#include "libgestures/actions/keysequence.h"
#include "libgestures/gestures/gesturerecognizer.h"

#include <QRegularExpression>
#include <QVector>

#include "yaml-cpp/yaml.h"

namespace YAML
{

struct range
{
    qreal min = 0.0;
    qreal max = 0.0;

    range() = default;
    range(qreal min, qreal max) {
        this->min = min;
        this->max = max;
    }
};

template <>
struct convert<range>
{
    static bool decode(const Node &node, range &range)
    {
        const auto rangeRaw = node.as<QString>();
        if (rangeRaw.contains("-")) {
            const auto split = rangeRaw.split("-");
            range.min = split[0].toDouble();
            range.max = split[1].toDouble();
        } else {
            range.min = range.max = rangeRaw.toDouble();
        }

        return true;
    }
};

template <>
struct convert<std::shared_ptr<libgestures::Condition>>
{
    static bool decode(const Node &node, std::shared_ptr<libgestures::Condition> &condition)
    {
        condition = std::make_shared<libgestures::Condition>();

        for (const auto &negatedRaw : node["negate"].as<QString>("").split(" ")) {
            if (negatedRaw.contains("window_class")) {
                condition->setNegateWindowClass(true);
            } else if (negatedRaw.contains("window_state")) {
                condition->setNegateWindowState(true);
            } else {
                throw Exception(node.Mark(), "Invalid condition property");
            }
        }

        const auto windowClass = node["window_class"];
        if (windowClass.IsDefined()) {
            condition->setWindowClass(windowClass.as<QRegularExpression>());
        }
        condition->setWindowState(node["window_state"].as<libgestures::WindowStates>(libgestures::WindowState::All));

        return true;
    }
};

template <>
struct convert<std::shared_ptr<libgestures::Gesture>>
{
    static bool decode(const Node &node, std::shared_ptr<libgestures::Gesture> &gesture)
    {
        const auto type = node["type"].as<QString>();
        if (type == "hold") {
            gesture = std::make_shared<libgestures::HoldGesture>();
        } else if (type == "pinch") {
            auto pinchGesture = new libgestures::PinchGesture;
            pinchGesture->setDirection(node["direction"].as<libgestures::PinchDirection>());
            gesture.reset(pinchGesture);
        } else if (type == "swipe") {
            auto swipeGesture = new libgestures::SwipeGesture;
            swipeGesture->setDirection(node["direction"].as<libgestures::SwipeDirection>());
            gesture.reset(swipeGesture);
        } else {
            throw Exception(node.Mark(), "Invalid gesture type");
        }

        const auto fingersNode = node["fingers"];
        if (!fingersNode) {
            throw Exception(node.Mark(), "Finger count not specified");
        }
        const auto fingersRaw = fingersNode.as<QString>();
        range fingers;
        if (fingersRaw.contains("-")) {
            const auto split = fingersRaw.split("-");
            fingers.min = split[0].toUInt();
            fingers.max = split[1].toUInt();
        } else {
            fingers.min = fingers.max = fingersRaw.toUInt();
        }

        const auto thresholdRaw = node["threshold"].as<QString>("");
        range threshold(-1, -1);
        if (thresholdRaw.contains("-")) {
            const auto split = thresholdRaw.split("-");
            threshold.min = split[0].toFloat();
            threshold.max = split[1].toFloat();
        } else {
            threshold.min = thresholdRaw.toFloat();
        }

        gesture->setFingers(fingers.min, fingers.max);
        gesture->setSpeed(node["speed"].as<libgestures::GestureSpeed>(libgestures::GestureSpeed::Any));
        gesture->setThresholds(threshold.min, threshold.max);

        for (const auto &conditionNode : node["conditions"]) {
            gesture->addCondition(conditionNode.as<std::shared_ptr<libgestures::Condition>>());
        }
        for (const auto &actionNode : node["actions"]) {
            gesture->addAction(actionNode.as<std::shared_ptr<libgestures::GestureAction>>());
        }

        return true;
    }
};

template <>
struct convert<std::shared_ptr<libgestures::GestureAction>>
{
    static bool decode(const Node &node, std::shared_ptr<libgestures::GestureAction> &action)
    {
        if (node["command"].IsDefined()) {
            auto commandAction = new libgestures::CommandGestureAction();
            commandAction->setCommand(node["command"].as<QString>());
            action.reset(commandAction);
        } else if (node["keyboard"].IsDefined()) {
            auto keySequenceAction = new libgestures::KeySequenceGestureAction;
            keySequenceAction->setSequence(node["keyboard"].as<QString>());
            action.reset(keySequenceAction);
        } else if (node["plasma_shortcut"].IsDefined()) {
            auto plasmaShortcutAction = new libgestures::KDEGlobalShortcutGestureAction;
            const auto split = node["plasma_shortcut"].as<QString>().split(",");
            plasmaShortcutAction->setComponent(split[0]);
            plasmaShortcutAction->setShortcut(split[1]);
            action.reset(plasmaShortcutAction);
        } else {
            throw Exception(node.Mark(), "Action has no valid action property");
        }

        const auto thresholdRaw = node["threshold"].as<QString>("");
        range threshold(-1, -1);
        if (thresholdRaw.contains("-")) {
            const auto split = thresholdRaw.split("-");
            threshold.min = split[0].toFloat();
            threshold.max = split[1].toFloat();
        } else {
            threshold.min = thresholdRaw.toFloat();
        }

        action->setWhen(node["when"].as<libgestures::When>(libgestures::When::Ended));
        action->setThresholds(threshold.min, threshold.max);
        action->setRepeatInterval(node["interval"].as<qreal>(0));
        action->setBlockOtherActions(node["block_other"].as<bool>(false));
        for (const auto &conditionNode : node["conditions"]) {
            action->addCondition(conditionNode.as<std::shared_ptr<libgestures::Condition>>());
        }

        return true;
    }
};

template <>
struct convert<std::shared_ptr<libgestures::GestureRecognizer>>
{
    static bool decode(const Node &node, std::shared_ptr<libgestures::GestureRecognizer> &gestureRecognizer)
    {
        const auto gesturesNode = node["gestures"];
        if (!gesturesNode.IsDefined()) {
            throw Exception(node.Mark(), "No gestures specified");
        }

        gestureRecognizer = std::make_unique<libgestures::GestureRecognizer>();
        for (const auto gestureNode : gesturesNode) {
            gestureRecognizer->registerGesture(gestureNode.as<std::shared_ptr<libgestures::Gesture>>());
        }

        return true;
    }
};

template <>
struct convert<libgestures::GestureSpeed>
{
    static bool decode(const Node &node, libgestures::GestureSpeed &speed)
    {
        const auto speedRaw = node.as<QString>();
        if (speedRaw == "fast") {
            speed = libgestures::GestureSpeed::Fast;
        } else if (speedRaw == "slow") {
            speed = libgestures::GestureSpeed::Slow;
        } else if (speedRaw == "any") {
            speed = libgestures::GestureSpeed::Any;
        } else {
            throw Exception(node.Mark(), "Invalid gesture speed");
        }

        return true;
    }
};

template <>
struct convert<libgestures::PinchDirection>
{
    static bool decode(const Node &node, libgestures::PinchDirection &direction)
    {
        const auto directionRaw = node.as<QString>();
        if (directionRaw == "inward") {
            direction = libgestures::PinchDirection::Inward;
        } else if (directionRaw == "outward") {
            direction = libgestures::PinchDirection::Outward;
        } else {
            throw Exception(node.Mark(), "Invalid pinch direction");
        }

        return true;
    }
};

template <>
struct convert<libgestures::SwipeDirection>
{
    static bool decode(const Node &node, libgestures::SwipeDirection &direction)
    {
        const auto directionRaw = node.as<QString>();
        if (directionRaw == "left") {
            direction = libgestures::SwipeDirection::Left;
        } else if (directionRaw == "right") {
            direction = libgestures::SwipeDirection::Right;
        } else if (directionRaw == "horizontal") {
            direction = libgestures::SwipeDirection::Horizontal;
        } else if (directionRaw == "up") {
            direction = libgestures::SwipeDirection::Up;
        } else if (directionRaw == "down") {
            direction = libgestures::SwipeDirection::Down;
        } else if (directionRaw == "vertical") {
            direction = libgestures::SwipeDirection::Vertical;
        } else {
            throw Exception(node.Mark(), "Invalid swipe direction");
        }

        return true;
    }
};

template <>
struct convert<libgestures::WindowStates>
{
    static bool decode(const Node &node, libgestures::WindowStates &windowStates)
    {
        windowStates = static_cast<libgestures::WindowState>(0);
        for (const auto &stateRaw : node.as<QString>().split(" ")) {
            if (stateRaw == "maximized") {
                windowStates |= libgestures::WindowState::Maximized;
            } else if (stateRaw == "fullscreen") {
                windowStates |= libgestures::WindowState::Fullscreen;
            } else {
                throw Exception(node.Mark(), "Invalid window state");
            }
        }

        return true;
    }
};

template <>
struct convert<libgestures::When>
{
    static bool decode(const Node &node, libgestures::When &when)
    {
        const auto raw = node.as<QString>();
        if (raw == "started") {
            when = libgestures::When::Started;
        } else if (raw == "updated") {
            when = libgestures::When::Updated;
        } else if (raw == "cancelled") {
            when = libgestures::When::Cancelled;
        } else if (raw == "ended") {
            when = libgestures::When::Ended;
        } else if (raw == "ended_cancelled") {
            when = libgestures::When::EndedOrCancelled;
        } else {
            throw Exception(node.Mark(), "Invalid action 'when' property value");
        }

        return true;
    }
};

template <>
struct convert<QString>
{
    static bool decode(const Node &node, QString &s)
    {
        s = QString::fromStdString(node.as<std::string>());
        return true;
    }
};

template <>
struct convert<QRegularExpression>
{
    static bool decode(const Node &node, QRegularExpression &regex)
    {
        regex = QRegularExpression(node.as<QString>());
        return true;
    }
};

}