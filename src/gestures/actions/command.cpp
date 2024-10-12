#include "command.h"

#include <utility>

CommandGestureAction::CommandGestureAction(QString command)
    : m_command(std::move(command))
{
}

void CommandGestureAction::execute()
{
    std::ignore = std::system((m_command + " &").toStdString().c_str());
}