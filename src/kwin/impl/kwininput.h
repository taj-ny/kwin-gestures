#pragma once

#include "core/inputdevice.h"
#include "libgestures/input.h"

class InputDevice : public KWin::InputDevice
{
public:
    QString sysName() const override;
    QString name() const override;
    bool isEnabled() const override;
    void setEnabled(bool enabled) override;
    KWin::LEDs leds() const override;
    void setLeds(KWin::LEDs leds) override;
    bool isKeyboard() const override;
    bool isPointer() const override;
    bool isTouchpad() const override;
    bool isTouch() const override;
    bool isTabletTool() const override;
    bool isTabletPad() const override;
    bool isTabletModeSwitch() const override;
    bool isLidSwitch() const override;
};

class KWinInput : public libgestures::Input
{
public:
    KWinInput();
    ~KWinInput() override;

    void keyboardKey(const uint32_t &key, const bool &state) override;

private:
    std::unique_ptr<InputDevice> m_device;
};