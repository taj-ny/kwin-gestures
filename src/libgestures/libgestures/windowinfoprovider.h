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

#include "yaml-cpp/yaml.h"
#include <QString>

namespace libgestures
{

enum WindowState {
    Maximized = 1u << 0,
    Fullscreen = 1u << 1,
    All = 0u - 1
};
Q_DECLARE_FLAGS(WindowStates, WindowState)
Q_DECLARE_OPERATORS_FOR_FLAGS(WindowStates)

class WindowInfoProvider;

class WindowInfo
{
public:
    WindowInfo(QString title, QString resourceClass, QString resourceName, WindowStates state);

    QString title() const
    {
        return m_title;
    };
    QString resourceClass() const
    {
        return m_resourceClass;
    };
    QString resourceName() const
    {
        return m_resourceName;
    };
    WindowStates state() const
    {
        return m_state;
    };

private:
    const QString m_title;
    const QString m_resourceClass;
    const QString m_resourceName;
    const WindowStates m_state;
};

/**
 * Provides information about the currently active window.
 */
class WindowInfoProvider
{
public:
    WindowInfoProvider() = default;
    virtual ~WindowInfoProvider() = default;

    /**
     * @return The window information of the currently active window, or @c std::nullopt if no window is currently
     * active.
     */
    virtual std::optional<const WindowInfo> activeWindow() const
    {
        return std::nullopt;
    };

    static WindowInfoProvider *implementation()
    {
        return s_implementation.get();
    }
    static void setImplementation(WindowInfoProvider *implementation);

private:
    static std::unique_ptr<WindowInfoProvider> s_implementation;
};

}