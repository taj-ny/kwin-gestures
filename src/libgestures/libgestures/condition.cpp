#include "condition.h"
#include "libgestures/libgestures.h"

namespace libgestures
{

bool Condition::isSatisfied() const
{
    const auto windowData = libgestures::windowInfoProvider()->activeWindow();
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

void Condition::setWindowClass(const QRegularExpression &windowClassRegex)
{
    m_windowClass = windowClassRegex;
}

void Condition::setWindowState(const WindowStates &windowState)
{
    m_windowState = windowState;
}

void Condition::setNegateWindowClass(const bool &negate)
{
    m_negateWindowClass = negate;
}

void Condition::setNegateWindowState(const bool &negate)
{
    m_negateWindowState = negate;
}

}