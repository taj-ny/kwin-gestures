#pragma once

#include "effect/effect.h"
#include "inputfilter.h"

class GesturesEffect : public KWin::Effect
{
public:
    GesturesEffect();
    ~GesturesEffect();

    static bool supported() { return true; };
    static bool enabledByDefault() { return true; };

    void reconfigure(ReconfigureFlags flags) override;

private:
    std::unique_ptr<GestureInputEventFilter> m_inputEventFilter;
};