#include "kdeglobalshortcut.h"
#include <QDBusInterface>

KDEGlobalShortcutGestureAction::KDEGlobalShortcutGestureAction(qreal repeatInterval, QString component, QString shortcut)
    : GestureAction(repeatInterval),
      m_component(std::move(component)),
      m_shortcut(std::move(shortcut))
{
}

void KDEGlobalShortcutGestureAction::execute()
{
    GestureAction::execute();
    QDBusInterface interface("org.kde.kglobalaccel", "/component/" + m_component, "org.kde.kglobalaccel.Component");
    interface.call("invokeShortcut", m_shortcut);
}