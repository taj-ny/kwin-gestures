#pragma once

#include "action.h"
#include <QString>

namespace libgestures
{

class CommandGestureAction : public GestureAction
{
public:
    void execute() override;

    void setCommand(const QString &command);
private:
    QString m_command;
};

} // namespace libgesture