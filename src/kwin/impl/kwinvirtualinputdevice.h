#pragma once

#include "core/inputdevice.h"
#include "virtualinputdevice.h"

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

class KWinVirtualInputDevice : public VirtualInputDevice
{
public:
    KWinVirtualInputDevice();
    ~KWinVirtualInputDevice() override;

    virtual void keyboardPress(const uint32_t &key) const;
    virtual void keyboardRelease(const uint32_t &key) const;
private:
    void sendKey(const uint32_t &key, const KWin::InputRedirection::KeyboardKeyState &state) const;

    std::unique_ptr<InputDevice> m_device;
};