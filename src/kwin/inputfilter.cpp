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

#include "inputfilter.h"

#include "libgestures/triggers/stroketrigger.h"

#include "utils.h"

#ifndef KWIN_6_3_OR_GREATER
#include "core/inputdevice.h"
#endif
#include "input_event.h"
#include "input_event_spy.h"
#include "wayland_server.h"

#ifndef KWIN_6_2_OR_GREATER
#define ENSURE_SESSION_UNLOCKED() \
    if (KWin::waylandServer()->isScreenLocked()) { \
        return false;           \
    }
#else
#define ENSURE_SESSION_UNLOCKED()
#endif

static uint32_t s_strokeRecordingTimeout = 250;

GestureInputEventFilter::GestureInputEventFilter()
#ifdef KWIN_6_2_OR_GREATER
    : KWin::InputEventFilter(KWin::InputFilterOrder::TabBox)
#endif
{
    m_strokeRecordingTimeoutTimer.setSingleShot(true);
    connect(&m_strokeRecordingTimeoutTimer, &QTimer::timeout, this, [this] {
        finishStrokeRecording();
    });
}

void GestureInputEventFilter::setMouseTriggerHandler(std::unique_ptr<libgestures::MouseTriggerHandler> handler)
{
    m_mouseTriggerHandler = std::move(handler);
}

void GestureInputEventFilter::setTouchpadTriggerHandler(std::unique_ptr<libgestures::TouchpadTriggerHandler> handler)
{
    m_touchpadTriggerHandler = std::move(handler);
}

bool GestureInputEventFilter::holdGestureBegin(int fingerCount, std::chrono::microseconds time)
{
    Q_UNUSED(time)
    ENSURE_SESSION_UNLOCKED();

    m_touchpadTriggerHandler->holdBegin(fingerCount);
    return false;
}

bool GestureInputEventFilter::holdGestureEnd(std::chrono::microseconds time)
{
    ENSURE_SESSION_UNLOCKED();

    if (m_touchpadTriggerHandler->holdEnd()) {
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

bool GestureInputEventFilter::holdGestureCancelled(std::chrono::microseconds time)
{
    Q_UNUSED(time)
    ENSURE_SESSION_UNLOCKED();

    m_touchpadTriggerHandler->holdCancel();
    return false;
}

bool GestureInputEventFilter::swipeGestureBegin(int fingerCount, std::chrono::microseconds time)
{
    Q_UNUSED(time)
    ENSURE_SESSION_UNLOCKED();

    if (m_isRecordingStroke) {
        return true;
    }

    m_touchpadTriggerHandler->swipeBegin(fingerCount);
    return false;
}

bool GestureInputEventFilter::swipeGestureUpdate(const QPointF &delta, std::chrono::microseconds time)
{
    ENSURE_SESSION_UNLOCKED();

    if (m_isRecordingStroke) {
        m_strokePoints.push_back(delta);
        return true;
    }

    return m_touchpadTriggerHandler->swipeUpdate(delta);
}

bool GestureInputEventFilter::swipeGestureEnd(std::chrono::microseconds time)
{
    ENSURE_SESSION_UNLOCKED();

    if (m_isRecordingStroke) {
        finishStrokeRecording();
        return true;
    }

    if (m_touchpadTriggerHandler->swipeEnd()) {
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

bool GestureInputEventFilter::swipeGestureCancelled(std::chrono::microseconds time)
{
    Q_UNUSED(time)
    ENSURE_SESSION_UNLOCKED();

    m_touchpadTriggerHandler->swipeCancel();
    return false;
}

bool GestureInputEventFilter::pinchGestureBegin(int fingerCount, std::chrono::microseconds time)
{
    Q_UNUSED(time)
    ENSURE_SESSION_UNLOCKED();

    m_pinchGestureActive = true;
    m_touchpadTriggerHandler->pinchBegin(fingerCount);
    return false;
}

bool GestureInputEventFilter::pinchGestureUpdate(qreal scale, qreal angleDelta, const QPointF &delta, std::chrono::microseconds time)
{
    Q_UNUSED(time)
    ENSURE_SESSION_UNLOCKED();

    // Two-finger pinch gestures may be at first incorrectly interpreted by libinput as scrolling. Libinput does
    // eventually correctly determine the gesture after a few events, but it doesn't send the GESTURE_PINCH_BEGIN event.
    if (!m_pinchGestureActive) {
        pinchGestureBegin(2, time);
    }

    return m_touchpadTriggerHandler->updatePinch(scale, angleDelta, delta);
}

bool GestureInputEventFilter::pinchGestureEnd(std::chrono::microseconds time)
{
    Q_UNUSED(time)
    ENSURE_SESSION_UNLOCKED();

    m_pinchGestureActive = false;
    if (m_touchpadTriggerHandler->pinchEnd()) {
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

bool GestureInputEventFilter::pinchGestureCancelled(std::chrono::microseconds time)
{
    Q_UNUSED(time)
    ENSURE_SESSION_UNLOCKED();

    m_pinchGestureActive = false;
    m_touchpadTriggerHandler->pinchCancel();
    return false;
}

#ifdef KWIN_6_3_OR_GREATER
bool GestureInputEventFilter::pointerMotion(KWin::PointerMotionEvent *event)
{
    const auto device = event->device;
    if (!device || !isMouse(event->device)) {
        return false;
    }

    if (m_isRecordingStroke) {
        m_strokePoints.push_back(event->delta);
        m_strokeRecordingTimeoutTimer.start(s_strokeRecordingTimeout);
    } else {
        m_mouseTriggerHandler->motion(event->delta);
    }
    return false;
}

bool GestureInputEventFilter::pointerButton(KWin::PointerButtonEvent *event)
{
    if (!event->device || !isMouse(event->device)) {
        return false;
    }

    return m_mouseTriggerHandler->button(event->button, event->nativeButton, event->state == PointerButtonStatePressed);
}

bool GestureInputEventFilter::keyboardKey(KWin::KeyboardKeyEvent *event)
{
    m_mouseTriggerHandler->keyboardKey(event->key, event->state == KeyboardKeyStatePressed);
    m_touchpadTriggerHandler->keyboardKey(event->key, event->state == KeyboardKeyStatePressed);
return false;
}
#endif

#ifdef KWIN_6_3_OR_GREATER
bool GestureInputEventFilter::pointerAxis(KWin::PointerAxisEvent *event)
{
    const auto device = event->device;
    const auto eventDelta = event->delta;
    const auto orientation = event->orientation;
    const auto inverted = event->inverted;
#else
bool GestureInputEventFilter::wheelEvent(KWin::WheelEvent *event)
{
    const auto device = event->device();
    const auto eventDelta = event->delta();
    const auto orientation = event->orientation();
    const auto inverted = event->inverted();
#endif

    ENSURE_SESSION_UNLOCKED();

    const auto mouse = isMouse(device);
    if (!device->isTouchpad() && !mouse) {
        return false;
    }

    auto delta = orientation == Qt::Orientation::Horizontal
        ? QPointF(eventDelta, 0)
        : QPointF(0, eventDelta);
    if (inverted) {
        delta *= -1;
    }

    if (mouse) {
        return m_mouseTriggerHandler->wheel(eventDelta, orientation);
    }

    if (m_isRecordingStroke) {
        m_strokePoints.push_back(delta);
        m_strokeRecordingTimeoutTimer.start(s_strokeRecordingTimeout);
        return true;
    }
    return m_touchpadTriggerHandler->scroll(eventDelta, orientation, inverted);
}

bool GestureInputEventFilter::isMouse(const KWin::InputDevice *device) const
{
    return device->isPointer() && !device->isTouch() && !device->isTouchpad();
}

void GestureInputEventFilter::recordStroke()
{
    m_isRecordingStroke = true;
}

void GestureInputEventFilter::finishStrokeRecording()
{
    m_isRecordingStroke = false;
    Q_EMIT strokeRecordingFinished(libgestures::Stroke(m_strokePoints));
    m_strokePoints.clear();
}

#include "moc_inputfilter.cpp"