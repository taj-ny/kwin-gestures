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

#include <libinputactions/input/emitter.h>

#include "core/inputdevice.h"
#include "kwin/input.h"
#include "input.h"

#ifdef KWIN_6_3_OR_GREATER
typedef KWin::KeyboardKeyState KeyboardKeyState;
#define KeyboardKeyStatePressed KWin::KeyboardKeyState::Pressed
#define KeyboardKeyStateReleased KWin::KeyboardKeyState::Released
#define PointerButtonStatePressed KWin::PointerButtonState::Pressed
#define PointerButtonStateReleased KWin::PointerButtonState::Released
#else
typedef KWin::InputRedirection::KeyboardKeyState KeyboardKeyState;
#define KeyboardKeyStatePressed KWin::InputRedirection::KeyboardKeyPressed
#define KeyboardKeyStateReleased KWin::InputRedirection::KeyboardKeyReleased
#define PointerButtonStatePressed KWin::InputRedirection::PointerButtonPressed
#define PointerButtonStateReleased KWin::InputRedirection::PointerButtonReleased
#endif

class InputDevice : public KWin::InputDevice
{
public:
    QString name() const override;
    bool isEnabled() const override;
    void setEnabled(bool enabled) override;
    bool isKeyboard() const override;
    bool isPointer() const override;
    bool isTouchpad() const override;
    bool isTouch() const override;
    bool isTabletTool() const override;
    bool isTabletPad() const override;
    bool isTabletModeSwitch() const override;
    bool isLidSwitch() const override;

#ifndef KWIN_6_3_OR_GREATER
    QString sysName() const override;
    KWin::LEDs leds() const override;
    void setLeds(KWin::LEDs leds) override;
#endif
};

class KWinInputEmitter : public libinputactions::InputEmitter
{
public:
    KWinInputEmitter();
    ~KWinInputEmitter() override;

    void keyboardKey(const uint32_t &key, const bool &state) override;
    void keyboardClearModifiers() override;

    void mouseButton(const uint32_t &button, const bool &state) override;
    void mouseMoveAbsolute(const QPointF &pos) override;
    void mouseMoveRelative(const QPointF &pos) override;

    bool isEmittingInput() const override;

private:
    KWin::PointerInputRedirection *m_pointer;
    KWin::KeyboardInputRedirection *m_keyboard;

    bool m_isEmittingInput{};
    std::unique_ptr<InputDevice> m_device;
};