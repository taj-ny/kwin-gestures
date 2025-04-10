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

#include "kwinwindowinfoprovider.h"
#include "effect/effecthandler.h"
#include "window.h"

std::optional<const libinputactions::WindowInfo> KWinWindowInfoProvider::activeWindow() const
{
    const auto window = KWin::effects->activeWindow();
    if (!window)
        return std::nullopt;

    libinputactions::WindowStates state = static_cast<libinputactions::WindowState>(0);
    if (window->isFullScreen())
        state = state | libinputactions::WindowState::Fullscreen;
    if (window->window()->maximizeMode() == KWin::MaximizeMode::MaximizeFull)
        state = state | libinputactions::WindowState::Maximized;

    return libinputactions::WindowInfo(window->caption(), window->window()->resourceClass(), window->window()->resourceName(), state);
}