#pragma once

#include "effect/effect.h"
#include "impl/kwininput.h"
#include "impl/kwinwindowinfoprovider.h"
#include "inputfilter.h"

#include <QFileSystemWatcher>

class Effect : public KWin::Effect
{
public:
    Effect();
    ~Effect() override;

    static bool supported()
    {
        return true;
    };
    static bool enabledByDefault()
    {
        return false;
    };

    void reconfigure(ReconfigureFlags flags) override;

private slots:
    void slotConfigFileChanged();
    void slotConfigDirectoryChanged();

private:
    void configureWatcher();

    void registerBuiltinGestures();

    bool m_autoReload = true;
    std::unique_ptr<GestureInputEventFilter> m_inputEventFilter = std::make_unique<GestureInputEventFilter>();
    QFileSystemWatcher m_configFileWatcher;
};