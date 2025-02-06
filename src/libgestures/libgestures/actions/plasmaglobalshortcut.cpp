#include "plasmaglobalshortcut.h"
#include <QDBusInterface>

namespace libgestures
{

bool PlasmaGlobalShortcutGestureAction::tryExecute()
{
    if (!GestureAction::tryExecute()) {
        return false;
    }

    QDBusInterface interface("org.kde.kglobalaccel", m_path, "org.kde.kglobalaccel.Component");
    interface.call("invokeShortcut", m_shortcut);
    return true;
}

PlasmaGlobalShortcutGestureAction &PlasmaGlobalShortcutGestureAction::setComponent(const QString &component)
{
    m_path = "/component/" + component;
    return *this;
}

PlasmaGlobalShortcutGestureAction &PlasmaGlobalShortcutGestureAction::setShortcut(const QString &shortcut)
{
    m_shortcut = shortcut;
    return *this;
}

}