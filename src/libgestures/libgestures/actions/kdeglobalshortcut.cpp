#include "kdeglobalshortcut.h"
#include <QDBusInterface>

namespace libgestures
{

bool KDEGlobalShortcutGestureAction::tryExecute()
{
    if (!GestureAction::tryExecute()) {
        return false;
    }

    QDBusInterface interface("org.kde.kglobalaccel", m_path, "org.kde.kglobalaccel.Component");
    interface.call("invokeShortcut", m_shortcut);
    return true;
}

void KDEGlobalShortcutGestureAction::setComponent(const QString &component)
{
    m_path = "/component/" + component;
}

void KDEGlobalShortcutGestureAction::setShortcut(const QString &shortcut)
{
    m_shortcut = shortcut;
}

}