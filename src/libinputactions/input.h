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
 * Provides access to input.
 */
class Input
{
public:
    Input() = default;
    virtual ~Input() = default;

    /**
     * @param state True to press, false to release.
     */
    virtual void keyboardKey(const uint32_t &key, const bool &state) { };

    /**
     * @return Currently pressed modifier keys on the keyboard, or Qt::KeyboardModifier::NoModifier if not implemented.
     */
    virtual Qt::KeyboardModifiers keyboardModifiers() const;
    virtual void keyboardClearModifiers() { };

    /**
     * @param state True to press, false to release.
     */
    virtual void mouseButton(const uint32_t &button, const bool &state) { };
    virtual void mouseMoveAbsolute(const QPointF &pos) { };
    virtual void mouseMoveRelative(const QPointF &pos) { };
    virtual Qt::MouseButtons mouseButtons() const;
    virtual QPointF mousePosition() const;

    virtual bool isSendingInput() const;

    static Input *implementation()
    {
        return s_implementation.get();
    }
    static void setImplementation(Input *implementation);

private:
    static std::unique_ptr<Input> s_implementation;
};

}