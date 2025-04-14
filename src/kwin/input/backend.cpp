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

#include "backend.h"
#include "emitter.h"
#include "utils.h"

#include <libinputactions/input/emitter.h>
#include <libinputactions/triggers/stroke.h>

#ifndef KWIN_6_3_OR_GREATER
#include "core/inputdevice.h"
#endif
#include "input_event.h"
#include "input_event_spy.h"
#include "wayland_server.h"

#ifndef KWIN_6_2_OR_GREATER
#define ENSURE_SESSION_UNLOCKED()                  \
    if (KWin::waylandServer()->isScreenLocked()) { \
        return false;                              \
    }
#else
#define ENSURE_SESSION_UNLOCKED()
#endif

static uint32_t s_strokeRecordingTimeout = 250;

KWinInputBackend::KWinInputBackend()
#ifdef KWIN_6_2_OR_GREATER
    : KWin::InputEventFilter(KWin::InputFilterOrder::TabBox)
#endif
{
    m_strokeRecordingTimeoutTimer.setSingleShot(true);
    connect(&m_strokeRecordingTimeoutTimer, &QTimer::timeout, this, [this] {
        finishStrokeRecording();
    });
}

bool KWinInputBackend::holdGestureBegin(int fingerCount, std::chrono::microseconds time)
{
    ENSURE_SESSION_UNLOCKED();

    m_touchpadTriggerHandler->handleHoldBeginEvent(fingerCount);
    return false;
}

bool KWinInputBackend::holdGestureEnd(std::chrono::microseconds time)
{
    ENSURE_SESSION_UNLOCKED();

    if (m_touchpadTriggerHandler->handleHoldEndEvent()) {
        KWin::input()->processSpies([&time](auto &&spy) {
            spy->holdGestureCancelled(time);
        });
        KWin::input()->processFilters([&time](auto &&filter) {
            return filter->holdGestureCancelled(time);
        });
        return true;
    }

    return false;
}

bool KWinInputBackend::holdGestureCancelled(std::chrono::microseconds time)
{
    ENSURE_SESSION_UNLOCKED();

    m_touchpadTriggerHandler->handleHoldCancelEvent();
    return false;
}

bool KWinInputBackend::swipeGestureBegin(int fingerCount, std::chrono::microseconds time)
{
    ENSURE_SESSION_UNLOCKED();

    if (m_isRecordingStroke) {
        return true;
    }

    m_touchpadTriggerHandler->handleSwipeBeginEvent(fingerCount);
    return false;
}

bool KWinInputBackend::swipeGestureUpdate(const QPointF &delta, std::chrono::microseconds time)
{
    ENSURE_SESSION_UNLOCKED();

    if (m_isRecordingStroke) {
        m_strokePoints.push_back(delta);
        return true;
    }

    return m_touchpadTriggerHandler->handleSwipeUpdateEvent(delta);
}

bool KWinInputBackend::swipeGestureEnd(std::chrono::microseconds time)
{
    ENSURE_SESSION_UNLOCKED();

    if (m_isRecordingStroke) {
        finishStrokeRecording();
        return true;
    }

    if (m_touchpadTriggerHandler->handleSwipeEndEvent()) {
        KWin::input()->processSpies([&time](auto &&spy) {
            spy->swipeGestureCancelled(time);
        });
        KWin::input()->processFilters([&time](auto &&filter) {
            return filter->swipeGestureCancelled(time);
        });
        return true;
    }

    return false;
}

bool KWinInputBackend::swipeGestureCancelled(std::chrono::microseconds time)
{
    ENSURE_SESSION_UNLOCKED();

    m_touchpadTriggerHandler->handleSwipeCancelEvent();
    return false;
}

bool KWinInputBackend::pinchGestureBegin(int fingerCount, std::chrono::microseconds time)
{
    ENSURE_SESSION_UNLOCKED();

    m_pinchGestureActive = true;
    m_touchpadTriggerHandler->handlePinchBeginEvent(fingerCount);
    return false;
}

bool KWinInputBackend::pinchGestureUpdate(qreal scale, qreal angleDelta, const QPointF &delta, std::chrono::microseconds time)
{
    ENSURE_SESSION_UNLOCKED();

    // Two-finger pinch gestures may be at first incorrectly interpreted by libinput as scrolling. Libinput does
    // eventually correctly determine the gesture after a few events, but it doesn't send the GESTURE_PINCH_BEGIN event.
    if (!m_pinchGestureActive) {
        pinchGestureBegin(2, time);
    }

    return m_touchpadTriggerHandler->handlePinchUpdateEvent(scale, angleDelta);
}

bool KWinInputBackend::pinchGestureEnd(std::chrono::microseconds time)
{
    ENSURE_SESSION_UNLOCKED();

    m_pinchGestureActive = false;
    if (m_touchpadTriggerHandler->handlePinchEndEvent()) {
        KWin::input()->processSpies([&time](auto &&spy) {
            spy->pinchGestureCancelled(time);
        });
        KWin::input()->processFilters([&time](auto &&filter) {
            return filter->pinchGestureCancelled(time);
        });
        return true;
    }

    return false;
}

bool KWinInputBackend::pinchGestureCancelled(std::chrono::microseconds time)
{
    ENSURE_SESSION_UNLOCKED();

    m_pinchGestureActive = false;
    m_touchpadTriggerHandler->handlePinchCancelEvent();
    return false;
}

#ifdef KWIN_6_3_OR_GREATER
bool KWinInputBackend::pointerMotion(KWin::PointerMotionEvent *event)
{
    const auto device = event->device;
    if (libinputactions::InputEmitter::instance()->isEmittingInput() || !device || !isMouse(event->device)) {
        return false;
    }

    if (m_isRecordingStroke) {
        m_strokePoints.push_back(event->delta);
        m_strokeRecordingTimeoutTimer.start(s_strokeRecordingTimeout);
    } else {
        m_mouseTriggerHandler->handleMotionEvent(event->delta);
    }
    return false;
}

bool KWinInputBackend::pointerButton(KWin::PointerButtonEvent *event)
{
    if (libinputactions::InputEmitter::instance()->isEmittingInput()|| !event->device || !isMouse(event->device)) {
        return false;
    }

    return m_mouseTriggerHandler->handleButtonEvent(event->button, event->nativeButton,
        event->state == PointerButtonStatePressed);
}

bool KWinInputBackend::keyboardKey(KWin::KeyboardKeyEvent *event)
{
    if (libinputactions::InputEmitter::instance()->isEmittingInput()) {
        return false;
    }

    m_mouseTriggerHandler->handleKeyEvent(event->key, event->state == KeyboardKeyStatePressed);
    m_touchpadTriggerHandler->handleKeyEvent(event->key, event->state == KeyboardKeyStatePressed);
    return false;
}
#endif

#ifdef KWIN_6_3_OR_GREATER
bool KWinInputBackend::pointerAxis(KWin::PointerAxisEvent *event)
{
    const auto device = event->device;
    const auto eventDelta = event->delta;
    const auto orientation = event->orientation;
    const auto inverted = event->inverted;
#else
bool KWinInputBackend::wheelEvent(KWin::WheelEvent *event)
{
    const auto device = event->device();
    const auto eventDelta = event->delta();
    const auto orientation = event->orientation();
    const auto inverted = event->inverted();
#endif

    ENSURE_SESSION_UNLOCKED();

    const auto mouse = isMouse(device);
    if (libinputactions::InputEmitter::instance()->isEmittingInput() || (!device->isTouchpad() && !mouse)) {
        return false;
    }

    auto delta = orientation == Qt::Orientation::Horizontal
        ? QPointF(eventDelta, 0)
        : QPointF(0, eventDelta);
    if (inverted) {
        delta *= -1;
    }

    if (mouse) {
        return m_mouseTriggerHandler->handleWheelEvent(eventDelta, orientation);
    }

    if (m_isRecordingStroke) {
        m_strokePoints.push_back(delta);
        m_strokeRecordingTimeoutTimer.start(s_strokeRecordingTimeout);
        return true;
    }
    return m_touchpadTriggerHandler->handleScrollEvent(eventDelta, orientation, inverted);
}

bool KWinInputBackend::isMouse(const KWin::InputDevice *device) const
{
    return device->isPointer() && !device->isTouch() && !device->isTouchpad();
}

void KWinInputBackend::recordStroke()
{
    m_isRecordingStroke = true;
}

void KWinInputBackend::finishStrokeRecording()
{
    m_isRecordingStroke = false;
    Q_EMIT strokeRecordingFinished(libinputactions::Stroke(m_strokePoints));
    m_strokePoints.clear();
}