#include "globalshortcut.h"
#include <QDBusInterface>

GlobalShortcutGestureAction::GlobalShortcutGestureAction(QString component, QString shortcut)
    : component(std::move(component)), shortcut(std::move(shortcut))
{
}

void GlobalShortcutGestureAction::execute()
{
    QDBusInterface interface("org.kde.kglobalaccel", "/component/" + component, "org.kde.kglobalaccel.Component");
    interface.call("invokeShortcut", shortcut);
}