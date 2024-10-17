#pragma once

#include "action.h"
#include <QString>

namespace libgestures
{

class CommandGestureAction : public GestureAction
{
public:
    CommandGestureAction(qreal repeatInterval, QString command);

    void execute() override;
private:
    const QString m_command;
};

} // namespace libgesture