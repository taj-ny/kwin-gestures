#pragma once

#include "animation.h"

#include <QRectF>

namespace libgestures
{

enum class GestureAnimation {
    None,
    Overlay,
    Window
};

/**
 * Manages animations for the currently active window.
 */
class AnimationHandler
{
public:
    AnimationHandler() = default;
    virtual ~AnimationHandler() = default;

    /**
     * Starts an animation that changes the window's geometry.
     * @param to The target window geometry.
     * @param resizeDuring Whether the window should be resized during the animation. This is necessary for some
     * animations (maximize, restore) to make them not look weird. Ignored for overlay animations.
     * @param keepLastFrame Whether the animation(s) should be kept active until ended manually by the animation
     * handler implementation. This is necessary to override some built-in animations like minimize. Ignored for
     * overlay animations.
     * @param fadeOut Whether to fade the window out. Ignored for overlay animations.
     */
    virtual void startGeometry(const GestureAnimation &type, const QRectF &to, const bool &resizeDuring = false, const bool &keepLastFrame = false, const bool &fadeOut = false);
    void update(const qreal &progress);
    virtual void end();
    virtual void cancel();

    virtual void repaint() { };

    const GestureAnimation &type() const;
    bool resizesWindow() const;

    const Animation<QRectF> &geometryAnimation() const;
    const Animation<qreal> &geometryOpacityAnimation() const;

    void setCurve(const QEasingCurve &curve);
    void setProgressCurve(const QEasingCurve &curve);
    void setAnimationDuration(const qreal &duration);

    const qreal &animationDuration() const;

    static AnimationHandler *implementation()
    {
        return s_implementation.get();
    }
    static void setImplementation(AnimationHandler *implementation);

    virtual QRectF windowGeometry() const;

protected:
    virtual void endedOrCancelled();

    GestureAnimation m_type;
    bool m_resizeDuring;
    bool m_keepLastFrame;
    bool m_fadeOut;

    Animation<QRectF> m_geometryAnimation;
    Animation<qreal> m_geometryOpacityAnimation;

    qreal m_animationDuration;

private:
    qreal m_progress;

    QEasingCurve m_curve = QEasingCurve::Linear;
    QEasingCurve m_curveProgress = QEasingCurve::InOutQuad;



    static std::unique_ptr<AnimationHandler> s_implementation;
};

}