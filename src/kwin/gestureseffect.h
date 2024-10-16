#pragma once

#include "effect/effect.h"
#include "impl/kdeglobalshortcutinvoker.h"
#include "impl/kwinvirtualinputdevice.h"
#include "impl/kwinwindowdataprovider.h"
#include "inputfilter.h"

class GesturesEffect : public KWin::Effect
{
public:
    GesturesEffect();
    ~GesturesEffect() override;

    static bool supported() { return true; };
    static bool enabledByDefault() { return false; };

    void reconfigure(ReconfigureFlags flags) override;

private:
    std::shared_ptr<GestureInputEventFilter> m_inputEventFilter;

    std::shared_ptr<GlobalShortcutInvoker> m_globalShortcutInvoker;
    std::shared_ptr<VirtualInputDevice> m_virtualInputDevice;
    std::shared_ptr<WindowDataProvider> m_windowDataProvider;
};