#pragma once

#include "effect/effect.h"
#include "input.h"
#include "inputfilter.h"
#include "gestures.h"

namespace KWin
{

class GesturesEffect : public Effect
{
    Q_OBJECT

public:
    GesturesEffect();
    ~GesturesEffect();

    static bool supported() { return true; };
    static bool enabledByDefault() { return true; };

    void reconfigure(ReconfigureFlags flags) override;

private:
    std::unique_ptr<GestureInputEventFilter> m_inputEventFilter;
};



} // namespace KWin
