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
    std::shared_ptr<GestureInputEventFilter> m_inputEventFilter = std::make_shared<GestureInputEventFilter>();
    std::shared_ptr<libgestures::Input> m_input = std::make_shared<KWinInput>();
    std::shared_ptr<libgestures::WindowInfoProvider> m_windowInfoProvider = std::make_shared<KWinWindowInfoProvider>();
};