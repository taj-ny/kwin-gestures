#include "input_event.h"
#include "input_event_spy.h"
#include "keyboard_input.h"
#include "virtualkeyboard.h"
#include "wayland/keyboard.h"
#include "xkb.h"

VirtualKeyboard::VirtualKeyboard()
{
    KWin::input()->addInputDevice(this);
}

VirtualKeyboard::~VirtualKeyboard()
{
    KWin::input()->removeInputDevice(this);
}

void VirtualKeyboard::press(quint32 key)
{
    sendKey(key, KWin::InputRedirection::KeyboardKeyState::KeyboardKeyPressed);
}

void VirtualKeyboard::release(quint32 key)
{
    sendKey(key, KWin::InputRedirection::KeyboardKeyState::KeyboardKeyReleased);
}

void VirtualKeyboard::sendKey(quint32 key, KWin::InputRedirection::KeyboardKeyState state)
{
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

QString VirtualKeyboard::sysName() const
{
    return "KWin Gestures Virtual Keyboard";
}

QString VirtualKeyboard::name() const
{
    return sysName();
}

bool VirtualKeyboard::isEnabled() const
{
    return true;
}

void VirtualKeyboard::setEnabled(bool enabled)
{
    Q_UNUSED(enabled)
}

KWin::LEDs VirtualKeyboard::leds() const
{
    return KWin::LEDs::fromInt(0);
}

void VirtualKeyboard::setLeds(KWin::LEDs leds)
{
    Q_UNUSED(leds)
}

bool VirtualKeyboard::isKeyboard() const
{
    return true;
}

bool VirtualKeyboard::isPointer() const
{
    return false;
}

bool VirtualKeyboard::isTouchpad() const
{
    return false;
}

bool VirtualKeyboard::isTouch() const
{
    return false;
}

bool VirtualKeyboard::isTabletTool() const
{
    return false;
}

bool VirtualKeyboard::isTabletPad() const
{
    return false;
}

bool VirtualKeyboard::isTabletModeSwitch() const
{
    return false;
}

bool VirtualKeyboard::isLidSwitch() const
{
    return false;
}
