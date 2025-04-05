/*
    Input Actions - Input handler that executes user-defined actions
    Copyright (C) 2024-2025 Marcin Woźniak

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "legacycondition.h"

namespace libgestures
{

bool LegacyCondition::satisfied() const
{
    const auto windowData = WindowInfoProvider::implementation()->activeWindow();
    if (!windowData)
        return false;

    return isWindowClassRegexSubConditionSatisfied(windowData.value())
        && isWindowStateSubConditionSatisfied(windowData.value());
}

bool LegacyCondition::isWindowClassRegexSubConditionSatisfied(const WindowInfo &data) const
{
    if (!m_windowClass || m_windowClass->pattern().isEmpty()) {
        return true;
    }

    return ((*m_windowClass).match(data.resourceClass()).hasMatch()
            || (*m_windowClass).match(data.resourceName()).hasMatch())
        == !m_negateWindowClass;
}

bool LegacyCondition::isWindowStateSubConditionSatisfied(const WindowInfo &data) const
{
    if (m_windowState == WindowState::All)
        return true;

    const bool satisfied =
        ((m_windowState & WindowState::Fullscreen) && (data.state() & WindowState::Fullscreen))
        || ((m_windowState & WindowState::Maximized) && (data.state() & WindowState::Maximized));
    return m_negateWindowState == !satisfied;
}

void LegacyCondition::setWindowClass(const QRegularExpression &windowClassRegex)
{
    m_windowClass = windowClassRegex;
}

void LegacyCondition::setWindowState(const WindowStates &windowState)
{
    m_windowState = windowState;
}

void LegacyCondition::setNegateWindowClass(const bool &negate)
{
    m_negateWindowClass = negate;
}

void LegacyCondition::setNegateWindowState(const bool &negate)
{
    m_negateWindowState = negate;
}

}