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

#include <libinputactions/handlers/mouse.h>
#include <libinputactions/handlers/touchpad.h>
#include <libinputactions/triggers/stroke.h>

namespace libinputactions
{

/**
 * Collects input events and forwards them to handlers.
 */
class InputBackend : public QObject
{
    Q_OBJECT

public:
    void recordStroke();

    void setMouseTriggerHandler(std::unique_ptr<MouseTriggerHandler> handler);
    void setTouchpadTriggerHandler(std::unique_ptr<TouchpadTriggerHandler> handler);

    static InputBackend *instance();
    static void setInstance(std::unique_ptr<InputBackend> instance);

signals:
    void strokeRecordingFinished(const Stroke &stroke);

protected:
    InputBackend();

    void finishStrokeRecording();

    std::unique_ptr<MouseTriggerHandler> m_mouseTriggerHandler = std::make_unique<MouseTriggerHandler>();
    std::unique_ptr<TouchpadTriggerHandler> m_touchpadTriggerHandler = std::make_unique<TouchpadTriggerHandler>();

    bool m_isRecordingStroke = false;
    std::vector<QPointF> m_strokePoints;
    QTimer m_strokeRecordingTimeoutTimer;

private:
    static std::unique_ptr<InputBackend> s_instance;
};

}