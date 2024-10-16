#include "input_event.h"
#include "input_event_spy.h"
#include "keyboard_input.h"
#include "kwinvirtualinputdevice.h"
#include "wayland/keyboard.h"
#include "xkb.h"

KWinVirtualInputDevice::KWinVirtualInputDevice()
    : m_device(std::make_unique<InputDevice>())
{
    KWin::input()->addInputDevice(m_device.get());
}

KWinVirtualInputDevice::~KWinVirtualInputDevice()
{
    KWin::input()->removeInputDevice(m_device.get());
}

void KWinVirtualInputDevice::keyboardPress(const uint32_t &key) const
{
    sendKey(key, KWin::InputRedirection::KeyboardKeyState::KeyboardKeyPressed);
}

void KWinVirtualInputDevice::keyboardRelease(const uint32_t &key) const
{
    sendKey(key, KWin::InputRedirection::KeyboardKeyState::KeyboardKeyReleased);
}

void KWinVirtualInputDevice::sendKey(const uint32_t &key, const KWin::InputRedirection::KeyboardKeyState &state) const
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
            m_device.get());
    event.setAccepted(false);
    event.setModifiersRelevantForGlobalShortcuts(globalShortcutsModifiers);

    KWin::input()->processSpies(std::bind(&KWin::InputEventSpy::keyEvent, std::placeholders::_1, &event));
    KWin::input()->processFilters(std::bind(&KWin::InputEventFilter::keyEvent, std::placeholders::_1, &event));

    xkb->forwardModifiers();
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