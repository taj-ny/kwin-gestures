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

#include "windowinfoprovider.h"

#include <utility>

namespace libgestures
{

WindowInfo::WindowInfo(QString title, QString resourceClass, QString resourceName, WindowStates state)
    : m_title(std::move(title))
    , m_resourceClass(std::move(resourceClass))
    , m_resourceName(std::move(resourceName))
    , m_state(state)
{
}

std::unique_ptr<WindowInfoProvider> WindowInfoProvider::s_implementation = std::make_unique<WindowInfoProvider>();
void WindowInfoProvider::setImplementation(WindowInfoProvider *implementation)
{
    s_implementation = std::unique_ptr<WindowInfoProvider>(implementation);
}

}