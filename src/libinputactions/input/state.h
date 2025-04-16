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

#include <memory>

#include <QPointF>

namespace libinputactions
{

/**
 * Provides read-only access to input state.
 */
class InputState
{
public:
    virtual ~InputState() = default;

    /**
     * @return Currently pressed keyboard modifiers or std::nullopt if not available.
     */
    virtual std::optional<Qt::KeyboardModifiers> keyboardModifiers() const;

    /**
     * @return Mouse position on the current screen ranging from (0,0) to (1,1) or std::nullopt if not available.
     */
    virtual std::optional<QPointF> mousePosition() const;

    static InputState *instance();
    static void setInstance(std::unique_ptr<InputState> instance);

protected:
    InputState() = default;

private:
    static std::unique_ptr<InputState> s_instance;
};

}