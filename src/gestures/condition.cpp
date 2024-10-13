#include "condition.h"
#include "effect/effecthandler.h"
#include "window.h"

Condition::Condition(bool negate, std::optional<QRegularExpression> windowClassRegex, std::optional<WindowState> windowState)
    : m_negate(negate),
      m_windowClassRegex(std::move(windowClassRegex)),
      m_windowState(windowState)
{
}

bool Condition::isSatisfied() const
{
    const auto window = KWin::effects->activeWindow();
    if (!window)
        return false;

    if ((m_windowClassRegex && !m_windowClassRegex.value().pattern().isEmpty())
        && !(m_windowClassRegex.value().match(window->window()->resourceClass()).hasMatch()
            || m_windowClassRegex.value().match(window->window()->resourceName()).hasMatch()))
        return m_negate;

    if (m_windowState) {
        bool satisfiesFullscreen = (m_windowState.value() & WindowState::Fullscreen) && window->isFullScreen();
        bool satisfiesMaximized = (m_windowState.value() & WindowState::Maximized)
            && window->frameGeometry() == KWin::effects->clientArea(KWin::MaximizeArea, KWin::effects->activeScreen(), KWin::effects->currentDesktop());

        return m_negate
            ? (!satisfiesFullscreen && !satisfiesMaximized)
            : (satisfiesFullscreen || satisfiesMaximized);
    }

    return !m_negate;
}