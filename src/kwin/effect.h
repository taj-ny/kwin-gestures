#pragma once

#include "effect/effect.h"
#include "impl/kwinvirtualinputdevice.h"
#include "impl/kwinwindowdataprovider.h"
#include "inputfilter.h"

class Effect : public KWin::Effect
{
public:
    Effect();
    ~Effect() override;

    static bool supported() { return true; };
    static bool enabledByDefault() { return false; };

    void reconfigure(ReconfigureFlags flags) override;
private:
    std::shared_ptr<GestureInputEventFilter> m_inputEventFilter = std::make_shared<GestureInputEventFilter>();
    std::shared_ptr<VirtualInputDevice> m_virtualInputDevice = std::make_shared<KWinVirtualInputDevice>();
    std::shared_ptr<WindowDataProvider> m_windowDataProvider = std::make_shared<KWinWindowDataProvider>();
};