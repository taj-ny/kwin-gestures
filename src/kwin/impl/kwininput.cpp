#include "kwininput.h"

#include "keyboard_input.h"
#include "pointer_input.h"

// TODO move key list
#include "libgestures/yaml_convert.h"

KWinInput::KWinInput()
    : m_device(std::make_unique<InputDevice>())
{
    m_input = KWin::input();
    m_input->addInputDevice(m_device.get());
    m_pointer = m_input->pointer();
    m_keyboard = m_input->keyboard();

    connect(m_input, &KWin::InputRedirection::keyboardModifiersChanged, this, &KWinInput::slotKeyboardModifiersChanged);
    connect(m_input, &KWin::InputRedirection::keyStateChanged, this, &KWinInput::slotKeyStateChanged);
}

KWinInput::~KWinInput()
{
    if (KWin::input()) {
        KWin::input()->removeInputDevice(m_device.get());
    }
}

void KWinInput::keyboardKey(const uint32_t &key, const bool &state)
{
    m_ignoreModifierUpdates = true;
    m_keyboard->processKey(key, state ? KeyboardKeyStatePressed : KeyboardKeyStateReleased, timestamp(), m_device.get());
    m_ignoreModifierUpdates = false;
}

Qt::KeyboardModifiers KWinInput::keyboardModifiers() const
{
    return m_modifiers;
}

void KWinInput::keyboardClearModifiers()
{
    // These events will belong to a different device, which wouldn't work with normal keys, but it works with modifiers.
    // The user should be able to start the gesture again while still having the modifiers pressed, so the previous
    // ones must be kept track of.
    keyboardKey(KEY_LEFTALT, false);
    keyboardKey(KEY_LEFTCTRL, false);
    keyboardKey(KEY_LEFTMETA, false);
    keyboardKey(KEY_LEFTSHIFT, false);
    keyboardKey(KEY_RIGHTALT, false);
    keyboardKey(KEY_RIGHTCTRL, false);
    keyboardKey(KEY_RIGHTMETA, false);
    keyboardKey(KEY_RIGHTSHIFT, false);
}

void KWinInput::mouseButton(const uint32_t &button, const bool &state)
{
    m_pointer->processButton(button, state ? PointerButtonStatePressed : PointerButtonStateReleased, timestamp(), m_device.get());
    m_pointer->processFrame(m_device.get());
}

void KWinInput::mouseMoveAbsolute(const QPointF &pos)
{
    m_pointer->processMotionAbsolute(pos, timestamp(), m_device.get());
    m_pointer->processFrame(m_device.get());
}

void KWinInput::mouseMoveRelative(const QPointF &pos)
{
    m_pointer->processMotion(pos, pos, timestamp(), m_device.get());
    m_pointer->processFrame(m_device.get());
}

void KWinInput::slotKeyboardModifiersChanged(Qt::KeyboardModifiers newMods, Qt::KeyboardModifiers oldMods)
{
    if (m_ignoreModifierUpdates) {
        return;
    }

    m_modifiers = newMods;
}

void KWinInput::slotKeyStateChanged(quint32 keyCode, KeyboardKeyState state)
{
    if (m_ignoreModifierUpdates || state != KeyboardKeyStateReleased) {
        return;
    }

    if (keyCode == KEY_LEFTALT || keyCode == KEY_RIGHTALT) {
        m_modifiers &= ~Qt::KeyboardModifier::AltModifier;
    } else if (keyCode == KEY_LEFTCTRL || keyCode == KEY_RIGHTCTRL) {
        m_modifiers &= ~Qt::KeyboardModifier::ControlModifier;
    } else if (keyCode == KEY_LEFTMETA || keyCode == KEY_RIGHTMETA) {
        m_modifiers &= ~Qt::KeyboardModifier::MetaModifier;
    } else if (keyCode == KEY_LEFTSHIFT || keyCode == KEY_RIGHTSHIFT) {
        m_modifiers &= ~Qt::KeyboardModifier::ShiftModifier;
    }
}

std::chrono::microseconds KWinInput::timestamp()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
}

QString InputDevice::name() const
{
    return "kwin_gestures";
}

bool InputDevice::isEnabled() const
{
    return true;
}

void InputDevice::setEnabled(bool enabled)
{
    Q_UNUSED(enabled)
}

bool InputDevice::isKeyboard() const
{
    return true;
}

bool InputDevice::isPointer() const
{
    return true;
}

bool InputDevice::isTouchpad() const
{
    return false;
}

bool InputDevice::isTouch() const
{
    return false;
}

bool InputDevice::isTabletTool() const
{
    return false;
}

bool InputDevice::isTabletPad() const
{
    return false;
}

bool InputDevice::isTabletModeSwitch() const
{
    return false;
}

bool InputDevice::isLidSwitch() const
{
    return false;
}

#ifndef KWIN_6_3_OR_GREATER
QString InputDevice::sysName() const
{
    return name();
}

KWin::LEDs InputDevice::leds() const
{
    return KWin::LEDs::fromInt(0);
}

void InputDevice::setLeds(KWin::LEDs leds)
{
    Q_UNUSED(leds)
}
#endif