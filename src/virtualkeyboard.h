#include "core/inputdevice.h"

/*
 * A virtual input device used for sending keystrokes.
 */
class VirtualInputDevice : public KWin::InputDevice
{
public:
    /// Adds the virtual input device.
    VirtualInputDevice();

    /// Removes the virtual input device.
    ~VirtualInputDevice() override;

    void keyboardPressKey(const quint32 &key);
    void keyboardReleaseKey(const quint32 &key);

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
    void sendKey(const quint32 &key, const KWin::InputRedirection::KeyboardKeyState &state);
};