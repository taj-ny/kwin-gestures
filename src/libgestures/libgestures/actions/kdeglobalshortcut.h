#pragma once

#include "action.h"
#include <QString>

namespace libgestures
{

class KDEGlobalShortcutGestureAction : public GestureAction
{
public:
    KDEGlobalShortcutGestureAction(qreal repeatInterval, QString component, QString shortcut);

    void execute() override;
private:
    const QString m_component;
    const QString m_shortcut;
};

} // namespace libgestuers