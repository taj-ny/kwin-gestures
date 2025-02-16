#pragma once

#include "impl/kwinanimationhandler.h"
#include "impl/kwininput.h"
#include "impl/kwinwindowinfoprovider.h"
#include "inputfilter.h"

#include "libgestures/gestures/builtingesture.h"

#include "core/output.h"
#include "effect/animationeffect.h"
#include "effect/timeline.h"

#include <QEasingCurve>
#include <QFileSystemWatcher>

#include <unordered_set>

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
    int requestedEffectChainPosition() const override
    {
        return 100;
    };

    void reconfigure(ReconfigureFlags flags) override;

    void prePaintScreen(KWin::ScreenPrePaintData &data, std::chrono::milliseconds presentTime) override;
    void paintScreen(const KWin::RenderTarget &renderTarget, const KWin::RenderViewport &viewport, int mask, const QRegion &region, KWin::Output *screen) override;

    void prePaintWindow(KWin::EffectWindow *w, KWin::WindowPrePaintData &data,
                        std::chrono::milliseconds presentTime) override;

    void
    paintWindow(const KWin::RenderTarget &renderTarget, const KWin::RenderViewport &viewport, KWin::EffectWindow *w,
                int mask, QRegion region, KWin::WindowPaintData &data) override;

private slots:
    void slotConfigFileChanged();
    void slotConfigDirectoryChanged();

private:
    void configureWatcher();

    void registerBuiltinGestures();

    KWin::EffectWindow *activeWindow();

    QRectF clientArea(const KWin::EffectWindow *w, const KWin::clientAreaOption &area) const;
    QRectF geometryRestore(const KWin::EffectWindow *w) const;
    bool isMaximized(const KWin::EffectWindow *w) const;

    bool m_autoReload = true;
    std::unique_ptr<GestureInputEventFilter> m_inputEventFilter = std::make_unique<GestureInputEventFilter>();
    QFileSystemWatcher m_configFileWatcher;

    qreal m_overlayOpacity = 1 / 3.0f;
    QColor m_overlayColor;

    std::chrono::milliseconds m_screenPresentTime;
    std::optional<QRect> m_previousRect;
    std::optional<QRect> m_currentRect;

    std::unordered_set<KWin::EffectWindow *> m_suppressAnimations;

    qreal m_animationQuality = 0;

    KWinAnimationHandler *m_animationHandler = new KWinAnimationHandler();
};