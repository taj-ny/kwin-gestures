#include "plasmaglobalshortcut.h"
#include <QDBusInterface>

namespace libgestures
{

PlasmaGlobalShortcutGestureAction::PlasmaGlobalShortcutGestureAction(const QString &component, const QString &shortcut)
    : m_path("/component/" + component)
    , m_shortcut(shortcut)
{
}

bool PlasmaGlobalShortcutGestureAction::tryExecute()
{
    if (!GestureAction::tryExecute()) {
        return false;
    }

    QDBusInterface interface("org.kde.kglobalaccel", m_path, "org.kde.kglobalaccel.Component");
    interface.call("invokeShortcut", m_shortcut);
    return true;
}

}