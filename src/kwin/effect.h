#pragma once

#include "impl/kwininput.h"
#include "impl/kwinwindowinfoprovider.h"
#include "inputfilter.h"

#include "core/output.h"
#include "effect/effect.h"

#include <QEasingCurve>
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

    void prePaintScreen(KWin::ScreenPrePaintData &data, std::chrono::milliseconds presentTime) override;
    void paintScreen(const KWin::RenderTarget &renderTarget, const KWin::RenderViewport &viewport, int mask, const QRegion &region, KWin::Output *screen) override;
    void postPaintScreen() override;

private slots:
    void slotConfigFileChanged();
    void slotConfigDirectoryChanged();

private:
    void configureWatcher();

    QRectF interpolate(const QRectF &from, const QRectF &to, qreal progress);
    qreal interpolate(const qreal &from, const qreal &to, qreal progress);

    void animateOpacity(const qreal &from, const qreal &to);
    void set(const QRectF &rect, const qreal &opacity);

    void registerBuiltinGestures();

    KWin::EffectWindow *activeWindow();

    /**
     * Creates a rectangle overlay animation for a built-in gesture. Use with BuiltinGesture::setAnimationFactory.
     * @param animation @c from is set to @c w->frameGeometry()
     */
    void createRectangleOverlayAnimation(
        std::vector<std::unique_ptr<libgestures::GestureAction>> &actions,
        const std::function<void(KWin::EffectWindow *w, QRectF &from, QRectF &to)> &animation
    );

    QRectF clientArea(const KWin::clientAreaOption &area) const;
    QRectF geometryRestore(const KWin::EffectWindow *w) const;
    bool isMaximized(const KWin::EffectWindow *w) const;

    bool m_autoReload = true;
    std::unique_ptr<GestureInputEventFilter> m_inputEventFilter = std::make_unique<GestureInputEventFilter>();
    QFileSystemWatcher m_configFileWatcher;

    std::chrono::milliseconds m_animationStart;
    bool m_hasAnimation = false;
    bool m_needsRepaints = false;
    qreal m_fromOpacity;
    qreal m_toOpacity;

    QRectF m_currentRect;
    qreal m_currentOpacity;

    QRectF m_from;
    QRectF m_to;

    QEasingCurve m_curve = QEasingCurve::Linear;
    QEasingCurve m_curveProgress = QEasingCurve::InOutQuad;

    qreal m_overlayOpacity = 1 / 3.0f;
    QColor m_overlayColor;

    std::chrono::milliseconds m_animationDuration{250};
};