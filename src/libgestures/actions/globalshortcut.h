#pragma once

#include "action.h"
#include "globalshortcutinvoker.h"
#include <QString>

class GlobalShortcutGestureAction : public GestureAction
{
public:
    GlobalShortcutGestureAction(qreal repeatInterval, std::shared_ptr<GlobalShortcutInvoker> globalShortcutInvoker, QString component, QString shortcut);

    void execute() override;
private:
    const std::shared_ptr<GlobalShortcutInvoker> m_globalShortcutInvoker;
    const QString m_component;
    const QString m_shortcut;
};