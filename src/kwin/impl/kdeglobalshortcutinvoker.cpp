#include "kdeglobalshortcutinvoker.h"
#include <QDBusInterface>

void KDEGlobalShortcutInvoker::invoke(const QString &component, const QString &shortcut) const
{
    QDBusInterface interface("org.kde.kglobalaccel", "/component/" + component, "org.kde.kglobalaccel.Component");
    interface.call("invokeShortcut", shortcut);
}