#include "handler.h"

#include <QDebug>

namespace libgestures
{

void AnimationHandler::startGeometry(const GestureAnimation &type, const QRectF &to, const bool &resizeDuring, const bool &keepLastFrame, const bool &fadeOut)
{
    m_type = type;
    m_resizeDuring = resizeDuring;
    m_keepLastFrame = true;
    m_fadeOut = fadeOut;

    m_geometryAnimation = Animation<QRectF>(windowGeometry(), to, m_curveProgress, m_animationDuration);
    m_geometryAnimation.setManual(true);
    m_geometryAnimation.start();

    switch (m_type) {
    case GestureAnimation::Overlay:
        m_geometryOpacityAnimation = Animation<qreal>(0, 1, m_curve,  m_animationDuration);
        m_geometryOpacityAnimation.start();
        break;

    case GestureAnimation::Window:
        if (m_fadeOut) {
            m_geometryOpacityAnimation = Animation<qreal>(1, 0, m_curveProgress, m_animationDuration, m_keepLastFrame);
            m_geometryOpacityAnimation.setManual(true);
            m_geometryOpacityAnimation.start();
        }
        break;
    }

    repaint();
}

void AnimationHandler::update(const qreal &progress)
{
    m_progress = progress;
    m_geometryAnimation.setManualProgress(progress);
    if (m_type == GestureAnimation::Window && m_fadeOut) {
        m_geometryOpacityAnimation.setManualProgress(progress);
    }
}

void AnimationHandler::end()
{
    switch (m_type) {
    case GestureAnimation::Overlay:
        m_geometryOpacityAnimation = Animation<qreal>(1, 0, m_curve,  m_animationDuration);
        m_geometryOpacityAnimation.start();

        if (m_progress >= 0.8) {
            m_geometryAnimation.setManualProgress(1);
        }
        break;

    case GestureAnimation::Window:
        m_geometryAnimation = Animation<QRectF>(m_geometryAnimation.value(m_progress), m_geometryAnimation.to(), m_curve, m_animationDuration, m_keepLastFrame);
        m_geometryAnimation.start();

        if (m_fadeOut) {
            m_geometryOpacityAnimation = Animation<qreal>(m_geometryOpacityAnimation.value(m_progress), 0, m_curve, m_animationDuration);
            m_geometryOpacityAnimation.start();
        }
    }

    repaint();
    endedOrCancelled();
}

void AnimationHandler::cancel()
{
    switch (m_type) {
    case libgestures::GestureAnimation::Overlay:
        m_geometryOpacityAnimation = Animation<qreal>(1, 0, m_curve, m_animationDuration);
        m_geometryOpacityAnimation.start();

        if (m_progress >= 0.8) {
            m_geometryAnimation.setManualProgress(1);
        }
        break;

    case libgestures::GestureAnimation::Window:
        m_geometryAnimation = Animation<QRectF>(m_geometryAnimation.value(m_progress), m_geometryAnimation.from(), m_curve,m_animationDuration);
        m_geometryAnimation.start();

        if (m_fadeOut) {
            m_geometryOpacityAnimation = Animation<qreal>(m_geometryOpacityAnimation.value(m_progress), 1, m_curve, m_animationDuration);
            m_geometryOpacityAnimation.start();
        }
    }

    repaint();
    endedOrCancelled();
}

void AnimationHandler::endedOrCancelled()
{
}

QRectF AnimationHandler::windowGeometry() const
{
    return {};
}

const Animation<QRectF> &AnimationHandler::geometryAnimation() const
{
    return m_geometryAnimation;
}

const Animation<qreal> &AnimationHandler::geometryOpacityAnimation() const
{
    return m_geometryOpacityAnimation;
}

const GestureAnimation &AnimationHandler::type() const
{
    return m_type;
}

bool AnimationHandler::resizesWindow() const
{
    return m_type == GestureAnimation::Window && m_resizeDuring;
}

void AnimationHandler::setCurve(const QEasingCurve &curve)
{
    m_curve = curve;
}

void AnimationHandler::setProgressCurve(const QEasingCurve &curve)
{
    m_curveProgress = curve;
}

void AnimationHandler::setAnimationDuration(const qreal &duration)
{
    m_animationDuration = duration;
}

void AnimationHandler::setImplementation(AnimationHandler *implementation)
{
    s_implementation = std::unique_ptr<AnimationHandler>(implementation);
}

std::unique_ptr<AnimationHandler> AnimationHandler::s_implementation = std::make_unique<AnimationHandler>();

}