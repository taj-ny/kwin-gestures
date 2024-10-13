#pragma once

#include "action.h"
#include <QString>

class CommandGestureAction : public GestureAction
{
public:
    explicit CommandGestureAction(QString command);

    void execute() const override;
private:
    const QString m_command;
};