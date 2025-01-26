#pragma once

#include "action.h"
#include <QString>

namespace libgestures
{

class CommandGestureAction : public GestureAction
{
public:
    bool tryExecute() override;

    void setCommand(const QString &command);
private:
    QString m_command;
};

}