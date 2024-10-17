#include "kdeglobalshortcut.h"
#include <QDBusInterface>

namespace libgestures
{

void KDEGlobalShortcutGestureAction::execute()
{
    GestureAction::execute();
    QDBusInterface interface("org.kde.kglobalaccel", "/component/" + m_component, "org.kde.kglobalaccel.Component");
    interface.call("invokeShortcut", m_shortcut);
}

void KDEGlobalShortcutGestureAction::setComponent(const QString &component)
{
    m_component = component;
}

void KDEGlobalShortcutGestureAction::setShortcut(const QString &shortcut)
{
    m_shortcut = shortcut;
}

} // namespace libgestures