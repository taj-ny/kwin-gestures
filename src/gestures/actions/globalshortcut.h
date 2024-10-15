#pragma once

#include "action.h"
#include <QString>

class GlobalShortcutGestureAction : public GestureAction
{
public:
    GlobalShortcutGestureAction(qreal repeatInterval, QString component, QString shortcut);

    void execute() override;
private:
    const QString m_component;
    const QString m_shortcut;
};