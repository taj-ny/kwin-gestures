#include "core/inputdevice.h"

class VirtualKeyboard : public KWin::InputDevice
{
public:
    VirtualKeyboard();
    ~VirtualKeyboard();

    void press(quint32 key);
    void release(quint32 key);

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

private:
    void sendKey(quint32 key, KWin::InputRedirection::KeyboardKeyState state);

};