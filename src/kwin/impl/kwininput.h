#pragma once

#include "libgestures/input.h"

#include "core/inputdevice.h"
#include "kwin/input.h"

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

class KWinInput : public QObject, public libgestures::Input
{
    Q_OBJECT

public:
    KWinInput();
    ~KWinInput() override;

    void keyboardKey(const uint32_t &key, const bool &state) override;
    Qt::KeyboardModifiers keyboardModifiers() const override;
    void keyboardClearModifiers() override;

    void mouseButton(const uint32_t &button, const bool &state) override;
    void mouseMoveAbsolute(const QPointF &pos) override;
    void mouseMoveRelative(const QPointF &pos) override;
    Qt::MouseButtons mouseButtons() const override;
    libgestures::Edges mouseScreenEdges(const qreal &threshold) const override;

    bool isSendingInput() const override;

private slots:
    void slotKeyboardModifiersChanged(Qt::KeyboardModifiers newMods, Qt::KeyboardModifiers oldMods);
    void slotKeyStateChanged(quint32 keyCode, KeyboardKeyState state);

private:
    KWin::InputRedirection *m_input;
    KWin::PointerInputRedirection *m_pointer;
    KWin::KeyboardInputRedirection *m_keyboard;
    std::unique_ptr<InputDevice> m_device;

    Qt::KeyboardModifiers m_modifiers = Qt::KeyboardModifier::NoModifier;
    bool m_ignoreModifierUpdates = false;
    bool m_isSendingInput = false;
};