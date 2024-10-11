#pragma once

#include "action.h"
#include <QString>

class GlobalShortcutGestureAction : public GestureAction
{
public:
    GlobalShortcutGestureAction(QString component, QString shortcut);

    QString component;
    QString shortcut;

    void execute() override;
};