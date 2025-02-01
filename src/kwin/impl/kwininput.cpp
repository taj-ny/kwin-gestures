#include "input_event.h"
#include "input_event_spy.h"
#include "keyboard_input.h"
#include "kwininput.h"
#include <linux/input-event-codes.h>
#include "wayland/keyboard.h"
#include "xkb.h"

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

void KWinInput::keyboardKey(const uint32_t &key, const bool &state)
{
    KWin::input()->keyboard()->processKey(
        key,
        state ? KWin::InputRedirection::KeyboardKeyPressed : KWin::InputRedirection::KeyboardKeyReleased,
        std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()),
        m_device.get()
    );
}

QString InputDevice::sysName() const
{
    return "KWin Gestures Virtual Keyboard";
}

QString InputDevice::name() const
{
    return sysName();
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
    return false;
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