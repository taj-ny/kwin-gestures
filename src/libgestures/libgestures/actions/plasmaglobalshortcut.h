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
    PlasmaGlobalShortcutGestureAction(const QString &component, const QString &shortcut);
    bool tryExecute() override;

private:
    const QString m_path;
    const QString m_shortcut;
};

}