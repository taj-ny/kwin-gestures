#pragma once

#include "effect/effect.h"
#include "impl/kwininput.h"
#include "impl/kwinwindowinfoprovider.h"
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
    std::unique_ptr<GestureInputEventFilter> m_inputEventFilter = std::make_unique<GestureInputEventFilter>();
};