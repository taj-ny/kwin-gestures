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

#pragma once

#include "condition.h"

#include "libgestures/windowinfoprovider.h"

#include <QRegularExpression>

namespace libgestures
{

// TODO Replace
class LegacyCondition : public Condition
{
public:
    LegacyCondition() = default;

    /**
     * @return @c false if no window is currently active. @c true if all specified subconditions are satisfied or if
     * none were specified.
     */
    bool satisfied() const override;

    /**
     * @param windowClassRegex If empty, the subcondition will always be satisfied.
     * @remark Requires @c libgestures::WindowInfoProvider to be implemented.
     */
    void setWindowClass(const QRegularExpression &windowClassRegex);

    /**
     * @remark Requires @c libgestures::WindowInfoProvider to be implemented.
     */
    void setWindowState(const WindowStates &windowState);

    void setNegateWindowClass(const bool &negate);
    void setNegateWindowState(const bool &negate);

private:
    bool isWindowClassRegexSubConditionSatisfied(const WindowInfo &data) const;
    bool isWindowStateSubConditionSatisfied(const WindowInfo &data) const;

    std::optional<QRegularExpression> m_windowClass = std::nullopt;
    bool m_negateWindowClass = false;

    WindowStates m_windowState = WindowState::All;
    bool m_negateWindowState = false;
};

}