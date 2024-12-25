#pragma once

#include "action.h"
#include <QString>

namespace libgestures
{

class KDEGlobalShortcutGestureAction : public GestureAction
{
public:
    bool tryExecute() override;

    void setComponent(const QString &component);
    void setShortcut(const QString &shortcut);
private:
    QString m_component;
    QString m_shortcut;
};

} // namespace libgestuers