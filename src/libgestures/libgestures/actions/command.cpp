#include "command.h"

namespace libgestures
{

void CommandGestureAction::execute()
{
    GestureAction::execute();
    std::ignore = std::system((m_command + " &").toStdString().c_str());
}

void CommandGestureAction::setCommand(const QString &command)
{
    m_command = command;
}

} // namespace libgesture