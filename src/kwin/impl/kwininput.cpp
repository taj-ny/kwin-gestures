#include "kwininput.h"

#include "input_event.h"
#include "input_event_spy.h"
#include "keyboard_input.h"
#include "pointer_input.h"
#include "wayland/keyboard.h"
#include "xkb.h"

#include <linux/input-event-codes.h>

KWinInput::KWinInput()
    : m_device(std::make_unique<InputDevice>())
{
    KWin::input()->addInputDevice(m_device.get());
}

KWinInput::~KWinInput()
{
    if (KWin::input()) {
        KWin::input()->removeInputDevice(m_device.get());
    }
}

void KWinInput::keyboardPress(const uint32_t &key)
{
    sendKey(key, KWin::InputRedirection::KeyboardKeyState::KeyboardKeyPressed);
    if (key == KEY_LEFTALT || key == KEY_RIGHTALT)
        m_modifiers |= Qt::KeyboardModifier::AltModifier;
    else if (key == KEY_LEFTSHIFT || key == KEY_RIGHTSHIFT)
        m_modifiers |= Qt::KeyboardModifier::ShiftModifier;
    else if (key == KEY_LEFTCTRL || key == KEY_RIGHTCTRL)
        m_modifiers |= Qt::KeyboardModifier::ControlModifier;
    else if (key == KEY_LEFTMETA || key == KEY_RIGHTMETA)
        m_modifiers |= Qt::KeyboardModifier::MetaModifier;
}

void KWinInput::keyboardRelease(const uint32_t &key)
{
    if (key == KEY_LEFTALT || key == KEY_RIGHTALT)
        m_modifiers &= ~Qt::KeyboardModifier::AltModifier;
    else if (key == KEY_LEFTSHIFT || key == KEY_RIGHTSHIFT)
        m_modifiers &= ~Qt::KeyboardModifier::ShiftModifier;
    else if (key == KEY_LEFTCTRL || key == KEY_RIGHTCTRL)
        m_modifiers &= ~Qt::KeyboardModifier::ControlModifier;
    else if (key == KEY_LEFTMETA || key == KEY_RIGHTMETA)
        m_modifiers &= ~Qt::KeyboardModifier::MetaModifier;
    sendKey(key, KWin::InputRedirection::KeyboardKeyState::KeyboardKeyReleased);
}

void KWinInput::mousePress(const uint32_t &button)
{
    sendMouseButton(button, KWin::InputRedirection::PointerButtonState::PointerButtonPressed);
}

void KWinInput::mouseRelease(const uint32_t &button)
{
    sendMouseButton(button, KWin::InputRedirection::PointerButtonState::PointerButtonReleased);
}

void KWinInput::sendKey(const uint32_t &key, const KWin::InputRedirection::KeyboardKeyState &state) const
{
    // https://invent.kde.org/plasma/kwin/-/blob/Plasma/6.2/src/keyboard_input.cpp
    const auto xkb = KWin::input()->keyboard()->xkb();
    xkb->updateKey(key, state);

    const xkb_keysym_t keySym = xkb->toKeysym(key);
    KWin::KeyEvent event(
            state == KWin::InputRedirection::KeyboardKeyPressed ? QEvent::KeyPress : QEvent::KeyRelease,
            xkb->toQtKey(keySym, key, m_modifiers),
            m_modifiers,
            key,
            keySym,
            xkb->toString(xkb->currentKeysym()),
            false,
            timestamp(),
            m_device.get());
    event.setAccepted(false);
    event.setModifiersRelevantForGlobalShortcuts(m_modifiers);

    KWin::input()->processSpies(std::bind(&KWin::InputEventSpy::keyEvent, std::placeholders::_1, &event));
    KWin::input()->processFilters(std::bind(&KWin::InputEventFilter::keyEvent, std::placeholders::_1, &event));

    xkb->forwardModifiers();
}

void KWinInput::sendMouseButton(const uint32_t &button, const KWin::InputRedirection::PointerButtonState &state) const
{
    KWin::input()->pointer()->processButton(button, state, timestamp(), m_device.get());
}

std::chrono::microseconds KWinInput::timestamp() const
{
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
}

QString InputDevice::sysName() const
{
    return "kwin_gestures";
}

QString InputDevice::name() const
{
    return "KWin Gestures";
}

bool InputDevice::isEnabled() const
{
    return true;
}

void InputDevice::setEnabled(bool enabled)
{
    Q_UNUSED(enabled)
}

KWin::LEDs InputDevice::leds() const
{
    return KWin::LEDs::fromInt(0);
}

void InputDevice::setLeds(KWin::LEDs leds)
{
    Q_UNUSED(leds)
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