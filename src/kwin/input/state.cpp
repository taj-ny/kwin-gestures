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

#include "core/output.h"
#include "pointer_input.h"
#include "workspace.h"

#include <linux/input-event-codes.h>

KWinInputState::KWinInputState()
{
    // KWin::InputRedirection::keyboardModifiersChanged sometimes doesn't get emitted and I have no idea why, this one
    // works though.
    connect(KWin::input(), &KWin::InputRedirection::keyStateChanged, this, &KWinInputState::slotKeyStateChanged);
}

std::optional<Qt::KeyboardModifiers> KWinInputState::keyboardModifiers() const
{
    return m_modifiers;
}

std::optional<QPointF> KWinInputState::mousePosition() const
{
    QPointF position;
    const auto rawPosition = KWin::input()->pointer()->pos();
    for (const auto &output : KWin::workspace()->outputs()) {
        const auto geometry = output->geometryF();
        if (!geometry.contains(rawPosition)) {
            continue;
        }

        const auto translatedPosition = rawPosition - geometry.topLeft();
        position.setX(translatedPosition.x() / geometry.width());
        position.setY(translatedPosition.y() / geometry.height());
    }
    return position;
}

void KWinInputState::slotKeyStateChanged(quint32 keyCode, KeyboardKeyState state)
{
    if (libinputactions::InputEmitter::instance()->isEmittingInput()) {
        return;
    }

    Qt::KeyboardModifier modifier;
    switch (keyCode) {
        case KEY_LEFTALT:
        case KEY_RIGHTALT:
            modifier = Qt::KeyboardModifier::AltModifier;
            break;
        case KEY_LEFTCTRL:
        case KEY_RIGHTCTRL:
            modifier = Qt::KeyboardModifier::ControlModifier;
            break;
        case KEY_LEFTMETA:
        case KEY_RIGHTMETA:
            modifier = Qt::KeyboardModifier::MetaModifier;
            break;
        case KEY_LEFTSHIFT:
        case KEY_RIGHTSHIFT:
            modifier = Qt::KeyboardModifier::ShiftModifier;
            break;
        default:
            return;
    }

    switch (state) {
        case KeyboardKeyStatePressed:
            m_modifiers |= modifier;
            break;
        case KeyboardKeyStateReleased:
            m_modifiers &= ~modifier;
            break;
        default:
            break;
    }
}