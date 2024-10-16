#include "globalshortcut.h"

GlobalShortcutGestureAction::GlobalShortcutGestureAction(qreal repeatInterval, std::shared_ptr<GlobalShortcutInvoker> globalShortcutInvoker, QString component, QString shortcut)
    : GestureAction(repeatInterval),
      m_globalShortcutInvoker(globalShortcutInvoker),
      m_component(std::move(component)),
      m_shortcut(std::move(shortcut))
{
}

void GlobalShortcutGestureAction::execute()
{
    GestureAction::execute();
    m_globalShortcutInvoker->invoke(m_component, m_shortcut);
}