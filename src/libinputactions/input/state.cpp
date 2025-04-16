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

#include "state.h"

namespace libinputactions
{

std::optional<Qt::KeyboardModifiers> InputState::keyboardModifiers() const
{
    return std::nullopt;
}

std::optional<QPointF> InputState::mousePosition() const
{
    return std::nullopt;
}

InputState *InputState::instance()
{
    return s_instance.get();
}

void InputState::setInstance(std::unique_ptr<InputState> instance)
{
    s_instance = std::move(instance);
}

std::unique_ptr<InputState> InputState::s_instance = std::unique_ptr<InputState>(new InputState);

}