#include "condition.h"

namespace libgestures
{

bool Condition::isSatisfied() const
{
    const auto windowData = WindowInfoProvider::implementation()->activeWindow();
    if (!windowData)
        return false;

    return isWindowClassRegexSubConditionSatisfied(windowData.value())
        && isWindowStateSubConditionSatisfied(windowData.value());
}

bool Condition::isWindowClassRegexSubConditionSatisfied(const WindowInfo &data) const
{
    if (!m_windowClass || m_windowClass->pattern().isEmpty()) {
        return true;
    }

    return ((*m_windowClass).match(data.resourceClass()).hasMatch()
            || (*m_windowClass).match(data.resourceName()).hasMatch())
        == !m_negateWindowClass;
}

bool Condition::isWindowStateSubConditionSatisfied(const WindowInfo &data) const
{
    if (m_windowState == WindowState::All)
        return true;

    const bool satisfied =
        ((m_windowState & WindowState::Fullscreen) && (data.state() & WindowState::Fullscreen))
        || ((m_windowState & WindowState::Maximized) && (data.state() & WindowState::Maximized));
    return m_negateWindowState == !satisfied;
}

Condition &Condition::setWindowClass(const QRegularExpression &windowClassRegex)
{
    m_windowClass = windowClassRegex;
    return *this;
}

Condition &Condition::setWindowState(const WindowStates &windowState)
{
    m_windowState = windowState;
    return *this;
}

Condition &Condition::setNegateWindowClass(const bool &negate)
{
    m_negateWindowClass = negate;
    return *this;
}

Condition &Condition::setNegateWindowState(const bool &negate)
{
    m_negateWindowState = negate;
    return *this;
}

}