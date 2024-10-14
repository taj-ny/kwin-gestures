#pragma once

#include "action.h"
#include <QString>

class GlobalShortcutGestureAction : public GestureAction
{
public:
    GlobalShortcutGestureAction(QString component, QString shortcut);

    void execute() const override;
private:
    const QString m_component;
    const QString m_shortcut;
};