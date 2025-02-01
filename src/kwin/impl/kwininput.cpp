#include "kwininput.h"

#include "keyboard_input.h"
#include "pointer_input.h"

KWinInput::KWinInput()
    : m_device(std::make_unique<InputDevice>())
{
    KWin::input()->addInputDevice(m_device.get());
    m_pointer = KWin::input()->pointer();
    m_keyboard = KWin::input()->keyboard();
}

KWinInput::~KWinInput()
{
    if (KWin::input()) {
        KWin::input()->removeInputDevice(m_device.get());
    }
}

void KWinInput::keyboardKey(const uint32_t &key, const bool &state)
{
    m_keyboard->processKey(
        key,
#ifdef KWIN_6_3_OR_GREATER
        state ? KWin::KeyboardKeyState::Pressed : KWin::KeyboardKeyState::Released,
#else
        state ? KWin::InputRedirection::KeyboardKeyPressed : KWin::InputRedirection::KeyboardKeyReleased,
#endif
        timestamp(),
        m_device.get()
    );
}

void KWinInput::mouseButton(const uint32_t &button, const bool &state)
{
    m_pointer->processButton(
        button,
#ifdef KWIN_6_3_OR_GREATER
        state ? KWin::PointerButtonState::Pressed : KWin::PointerButtonState::Released,
#else
        state ? KWin::InputRedirection::PointerButtonPressed : KWin::InputRedirection::PointerButtonReleased,
#endif
        timestamp(),
        m_device.get()
    );
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