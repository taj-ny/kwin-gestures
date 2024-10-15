/*
    KWin - the KDE window manager

    SPDX-FileCopyrightText: 2013, 2016 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "input_event.h"
#include "input_event_spy.h"
#include "keyboard_input.h"
#include "virtualkeyboard.h"
#include "wayland/keyboard.h"
#include "xkb.h"

VirtualInputDevice::VirtualInputDevice()
{
    KWin::input()->addInputDevice(this);
}

VirtualInputDevice::~VirtualInputDevice()
{
    KWin::input()->removeInputDevice(this);
}

void VirtualInputDevice::keyboardPressKey(const quint32 &key)
{
    sendKey(key, KWin::InputRedirection::KeyboardKeyState::KeyboardKeyPressed);
}

void VirtualInputDevice::keyboardReleaseKey(const quint32 &key)
{
    sendKey(key, KWin::InputRedirection::KeyboardKeyState::KeyboardKeyReleased);
}

void VirtualInputDevice::sendKey(const quint32 &key, const KWin::InputRedirection::KeyboardKeyState &state)
{
    // https://invent.kde.org/plasma/kwin/-/blob/Plasma/6.2/src/keyboard_input.cpp
    const auto xkb = KWin::input()->keyboard()->xkb();
    xkb->updateKey(key, state);

    const xkb_keysym_t keySym = xkb->toKeysym(key);
    const Qt::KeyboardModifiers globalShortcutsModifiers = xkb->modifiersRelevantForGlobalShortcuts(key);
    KWin::KeyEvent event(
        state == KWin::InputRedirection::KeyboardKeyPressed ? QEvent::KeyPress : QEvent::KeyRelease,
        xkb->toQtKey(keySym, key, globalShortcutsModifiers ? Qt::ControlModifier : Qt::KeyboardModifiers()),
        Qt::KeyboardModifier::NoModifier,
        key,
        keySym,
        xkb->toString(xkb->currentKeysym()),
        false,
        std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()),
        this);
    event.setAccepted(false);
    event.setModifiersRelevantForGlobalShortcuts(globalShortcutsModifiers);

    KWin::input()->processSpies(std::bind(&KWin::InputEventSpy::keyEvent, std::placeholders::_1, &event));
    KWin::input()->processFilters(std::bind(&KWin::InputEventFilter::keyEvent, std::placeholders::_1, &event));

    xkb->forwardModifiers();
}

QString VirtualInputDevice::sysName() const
{
    return "KWin Gestures Virtual Keyboard";
}

QString VirtualInputDevice::name() const
{
    return sysName();
}

bool VirtualInputDevice::isEnabled() const
{
    return true;
}

void VirtualInputDevice::setEnabled(bool enabled)
{
    Q_UNUSED(enabled)
}

KWin::LEDs VirtualInputDevice::leds() const
{
    return KWin::LEDs::fromInt(0);
}

void VirtualInputDevice::setLeds(KWin::LEDs leds)
{
    Q_UNUSED(leds)
}

bool VirtualInputDevice::isKeyboard() const
{
    return true;
}

bool VirtualInputDevice::isPointer() const
{
    return false;
}

bool VirtualInputDevice::isTouchpad() const
{
    return false;
}

bool VirtualInputDevice::isTouch() const
{
    return false;
}

bool VirtualInputDevice::isTabletTool() const
{
    return false;
}

bool VirtualInputDevice::isTabletPad() const
{
    return false;
}

bool VirtualInputDevice::isTabletModeSwitch() const
{
    return false;
}

bool VirtualInputDevice::isLidSwitch() const
{
    return false;
}
