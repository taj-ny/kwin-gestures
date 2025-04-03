#pragma once

#include "libgestures/handlers/motiontriggerhandler.h"

namespace libgestures
{

class MouseTriggerHandler : public MotionTriggerHandler
{
public:
    MouseTriggerHandler();

    bool button(const Qt::MouseButton &button, const quint32 &nativeButton, const bool &state);
    void motion(const QPointF &delta);
    bool wheel(const qreal &delta, const Qt::Orientation &orientation);

    std::unique_ptr<TriggerActivateEvent> createActivateEvent() const override;
    std::unique_ptr<TriggerEndEvent> createEndEvent() const override;

private:
    bool shouldBlockMouseButton(const Qt::MouseButton &button);
    void pressBlockedMouseButtons();

    /**
     * Used to wait until all mouse buttons have been pressed to avoid conflicts with gestures that require more than
     * one button.
     */
    QTimer m_pressTimeoutTimer;
    qreal m_pressTimeout = 50;
    QTimer m_motionTimeoutTimer;
    qreal m_motionTimeout = 200;
//    GestureBeginEvent m_data;
    bool m_instantPress = false;
    qreal m_mouseMotionSinceButtonPress = 0;
    bool m_hadMouseGestureSinceButtonPress = false;
    QList<quint32> m_blockedMouseButtons;
};

}