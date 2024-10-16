#include "condition.h"

Condition::Condition(std::shared_ptr<WindowDataProvider> windowDataProvider, bool negate, std::optional<QRegularExpression> windowClassRegex, std::optional<WindowState> windowState)
    : m_windowDataProvider(windowDataProvider),
      m_negate(negate),
      m_windowClassRegex(std::move(windowClassRegex)),
      m_windowState(windowState)
{
}

bool Condition::isSatisfied() const
{
    const auto windowData = m_windowDataProvider->getDataForActiveWindow();
    if (!windowData)
        return false;

    if ((m_windowClassRegex && !m_windowClassRegex.value().pattern().isEmpty())
        && !(m_windowClassRegex.value().match(windowData->resourceClass()).hasMatch()
            || m_windowClassRegex.value().match(windowData->resourceName()).hasMatch()))
        return m_negate;

    if (m_windowState) {
        bool satisfiesFullscreen = (m_windowState.value() & WindowState::Fullscreen) && (windowData->state() & WindowState::Fullscreen);
        bool satisfiesMaximized = (m_windowState.value() & WindowState::Maximized) && (windowData->state() & WindowState::Maximized);

        return m_negate
            ? (!satisfiesFullscreen && !satisfiesMaximized)
            : (satisfiesFullscreen || satisfiesMaximized);
    }

    return !m_negate;
}