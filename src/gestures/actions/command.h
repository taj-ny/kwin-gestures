#pragma once

#include "action.h"
#include <QString>

class CommandGestureAction : public GestureAction
{
public:
    CommandGestureAction(QString command);

    QString command;

    void execute() override;
};