#pragma once

#include "action.h"
#include <QString>

namespace libgestures
{

/**
 * Invokes a Plasma global shortcut.
 */
class PlasmaGlobalShortcutGestureAction : public GestureAction
{
public:
    bool tryExecute() override;
    PlasmaGlobalShortcutGestureAction &setComponent(const QString &component);
    PlasmaGlobalShortcutGestureAction &setShortcut(const QString &shortcut);

private:
    QString m_path;
    QString m_shortcut;
};

}