#include "effect/effecthandler.h"
#include "gestureseffect.h"
#include "kwin/input.h"
#include "gestures.h"
#include "globalshortcuts.h"
#include "wayland_server.h"
#include "window.h"
#include <QDBusInterface>

namespace KWin
{

GesturesEffect::GesturesEffect()
    : m_inputEventFilter(new GestureInputEventFilter())
{
    input()->prependInputEventFilter(m_inputEventFilter.get());

    reconfigure(ReconfigureAll);
}

GesturesEffect::~GesturesEffect()
{
    input()->uninstallInputEventFilter(m_inputEventFilter.get());
}

void GesturesEffect::reconfigure(ReconfigureFlags flags)
{
    m_inputEventFilter->unregisterGestures();

    const KConfig config("kwingesturesrc", KConfig::SimpleConfig);
    const auto gesturesGroup = config.group("Gestures");

    for (const auto &gestureGroupName : gesturesGroup.groupList())
    {
        const auto gestureGroup = gesturesGroup.group(gestureGroupName);

        const auto type = gestureGroup.readEntry("Type", "Swipe");

        const int fingerCount = gestureGroup.readEntry("FingerCount", -1);
        uint minimumFingerCount;
        uint maximumFingerCount;
        if (fingerCount == -1)
        {
            minimumFingerCount = gestureGroup.readEntry("MinimumFingerCount", 3);
            maximumFingerCount = gestureGroup.readEntry("MaximumFingerCount", 3);
        }
        else
        {
            minimumFingerCount = maximumFingerCount = fingerCount;
        }

        const bool triggerAfterReachingThreshold = gestureGroup.readEntry("TriggerAfterReachingThreshold", false);

        Gesture *gesture;
        if (type == "Swipe")
        {
            const auto directionString = gestureGroup.readEntry("Direction", "Left");
            auto direction = SwipeDirection::Left;
            if (directionString == "Right")
                direction = SwipeDirection::Right;
            else if (directionString == "Up")
                direction = SwipeDirection::Up;
            else if (directionString == "Down")
                direction = SwipeDirection::Down;

            const qreal thresholdX = gestureGroup.readEntry("SwipeThresholdX", 0.0);
            const qreal thresholdY = gestureGroup.readEntry("SwipeThresholdY", 0.0);
            const QPointF threshold(thresholdX, thresholdY);

            SwipeGesture *swipeGesture = new SwipeGesture(direction, minimumFingerCount, maximumFingerCount, triggerAfterReachingThreshold, threshold);
            m_inputEventFilter->registerTouchpadGesture(swipeGesture);
            gesture = swipeGesture;
        }
        else if (type == "Pinch")
        {
            const auto direction = gestureGroup.readEntry("Direction", "Contracting") == "Contracting"
                ? PinchDirection::Contracting
                : PinchDirection::Expanding;
            const qreal threshold = gestureGroup.readEntry("PinchThreshold", 1.0);

            PinchGesture *pinchGesture = new PinchGesture(direction, minimumFingerCount, maximumFingerCount, triggerAfterReachingThreshold, threshold);
            m_inputEventFilter->registerTouchpadGesture(pinchGesture);
            gesture = pinchGesture;
        }

        const QString actionString = gestureGroup.readEntry("Action", "None");
        std::function<void()> action = [] { };
        if (actionString == "GlobalShortcut") {
            const QString component = gestureGroup.readEntry("ActionGlobalShortcutComponent", "");
            const QString shortcut = gestureGroup.readEntry("ActionGlobalShortcutShortcut", "");

            action = [=] {
                QDBusInterface interface("org.kde.kglobalaccel", "/component/" + component, "org.kde.kglobalaccel.Component");
                interface.call("invokeShortcut", shortcut);
            };
        }

        connect(gesture, &Gesture::triggered, action);
    }
}

}

#include "moc_gestureseffect.cpp"