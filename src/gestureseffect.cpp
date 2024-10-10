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

    const KConfig config("testrc", KConfig::SimpleConfig);
    const auto gesturesGroup = config.group("Gestures");

    for (const auto &gestureGroupName : gesturesGroup.groupList())
    {
        const auto gestureGroup = gesturesGroup.group(gestureGroupName);

        const QString type = gestureGroup.readEntry("Type", "Swipe");

        const uint minimumFingerCount = gestureGroup.readEntry("MinimumFingerCount", 3);
        const uint maximumFingerCount = gestureGroup.readEntry("MaximumFingerCount", 3);

        Gesture *gesture;
        {
            const QString directionString = gestureGroup.readEntry("Direction", "Left");
            SwipeDirection direction = SwipeDirection::Left;
            if (directionString == "Right")
                direction = SwipeDirection::Right;
            else if (directionString == "Up")
                direction = SwipeDirection::Up;
            else if (directionString == "Down")
                direction = SwipeDirection::Down;

            SwipeGesture *swipeGesture = new SwipeGesture(direction, minimumFingerCount, maximumFingerCount);
            m_inputEventFilter->registerTouchpadGesture(swipeGesture);
            gesture = swipeGesture;
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