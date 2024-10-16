#include "condition.h"

Condition::Condition(std::shared_ptr<WindowDataProvider> windowDataProvider)
    : m_windowDataProvider(windowDataProvider)
{
}

bool Condition::isSatisfied() const
{
    const auto windowData = m_windowDataProvider->getDataForActiveWindow();
    if (!windowData)
        return false;

    return isWindowClassRegexSubConditionSatisfied(windowData.value())
        && isWindowStateSubConditionSatisfied(windowData.value());
}

void Condition::setNegate(const bool &negate)
{
    m_negate = negate;
}

void Condition::setWindowClassRegex(const QRegularExpression &windowClassRegex)
{
    m_windowClassRegex = windowClassRegex;
}

void Condition::setWindowState(const WindowState &windowState)
{
    m_windowState = windowState;
}

bool Condition::isWindowClassRegexSubConditionSatisfied(const WindowData &data) const
{
    if (!m_windowClassRegex || m_windowClassRegex->pattern().isEmpty())
        return true;

    if (m_windowClassRegex.value().match(data.resourceClass()).hasMatch()
        || m_windowClassRegex.value().match(data.resourceName()).hasMatch())
        return !m_negate;

    return m_negate;
}

bool Condition::isWindowStateSubConditionSatisfied(const WindowData &data) const
{
    if (!m_windowState)
        return true;

    const bool satisfied =
        ((m_windowState.value() & WindowState::Fullscreen) && (data.state() & WindowState::Fullscreen))
        || ((m_windowState.value() & WindowState::Maximized) && (data.state() & WindowState::Maximized));
    return m_negate == !satisfied;
}