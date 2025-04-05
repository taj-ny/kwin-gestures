/*
    Input Actions - Input handler that executes user-defined actions
    Copyright (C) 2024-2025 Marcin Woźniak

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "input.h"

#include "impl/kwininput.h"

#include "libgestures/handlers/mousetriggerhandler.h"
#include "libgestures/handlers/touchpadtriggerhandler.h"
#include "libgestures/triggers/stroketrigger.h"

#include <QTimer>

/**
 * Captures input events, forwards them to the proper instance of GestureHandler, and blocks them if necessary.
 *
 * Installed before GlobalShortcutFilter, which is responsible for handling touchpad gestures.
 *
 * @remark If KWin version <=6.1.90, this filter is installed as the first one. For this reason, all methods that
 * process events must not do anything if the session is locked and must pass the event to the next filter. On later
 * versions, it's installed right before GlobalShortcutFilter.
 *
 * @returns All methods that process events should return @c true to stop further event processing, @c false to pass to
 * next filter.
 */
class GestureInputEventFilter : public QObject, public KWin::InputEventFilter
{
    Q_OBJECT
public:
    GestureInputEventFilter();

    void setMouseTriggerHandler(std::unique_ptr<libgestures::MouseTriggerHandler> handler);
    void setTouchpadTriggerHandler(std::unique_ptr<libgestures::TouchpadTriggerHandler> handler);

    bool holdGestureBegin(int fingerCount, std::chrono::microseconds time) override;
    bool holdGestureEnd(std::chrono::microseconds time) override;
    bool holdGestureCancelled(std::chrono::microseconds time) override;

    bool swipeGestureBegin(int fingerCount, std::chrono::microseconds time) override;
    bool swipeGestureUpdate(const QPointF &delta, std::chrono::microseconds time) override;
    bool swipeGestureEnd(std::chrono::microseconds time) override;
    bool swipeGestureCancelled(std::chrono::microseconds time) override;

    bool pinchGestureBegin(int fingerCount, std::chrono::microseconds time) override;
    bool pinchGestureUpdate(qreal scale, qreal angleDelta, const QPointF &delta, std::chrono::microseconds time) override;
    bool pinchGestureEnd(std::chrono::microseconds time) override;
    bool pinchGestureCancelled(std::chrono::microseconds time) override;

#ifdef KWIN_6_3_OR_GREATER
    bool pointerMotion(KWin::PointerMotionEvent *event) override;
    bool pointerButton(KWin::PointerButtonEvent *event) override;
    bool pointerAxis(KWin::PointerAxisEvent *event) override;

    bool keyboardKey(KWin::KeyboardKeyEvent *event) override;
#else
    bool wheelEvent(KWin::WheelEvent *event) override;
#endif

    void recordStroke();

signals:
    void strokeRecordingFinished(const libgestures::Stroke &stroke);

private:
    bool isMouse(const KWin::InputDevice *device) const;

    void finishStrokeRecording();

    std::unique_ptr<libgestures::MouseTriggerHandler> m_mouseTriggerHandler = std::make_unique<libgestures::MouseTriggerHandler>();
    std::unique_ptr<libgestures::TouchpadTriggerHandler> m_touchpadTriggerHandler = std::make_unique<libgestures::TouchpadTriggerHandler>();

    bool m_pinchGestureActive = false;

    bool m_isRecordingStroke = false;
    std::vector<QPointF> m_strokePoints;
    QTimer m_strokeRecordingTimeoutTimer;
};