#include "command.h"

#include <utility>

CommandGestureAction::CommandGestureAction(qreal repeatInterval, QString command)
    : GestureAction(repeatInterval),
      m_command(std::move(command))
{
}

void CommandGestureAction::execute()
{
    GestureAction::execute();
    std::ignore = std::system((m_command + " &").toStdString().c_str());
}