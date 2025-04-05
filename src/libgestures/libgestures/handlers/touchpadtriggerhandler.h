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

#include "libgestures/handlers/multitouchmotiontriggerhandler.h"

namespace libgestures
{

class TouchpadTriggerHandler : public MultiTouchMotionTriggerHandler
{
public:
    TouchpadTriggerHandler();

    bool holdBegin(const uint8_t &fingers);
    bool holdEnd();
    bool holdCancel();

    bool pinchBegin(const uint8_t &fingers);
    bool pinchUpdate(const qreal &scale, const qreal &angleDelta, const QPointF &delta);
    bool pinchEnd();
    bool pinchCancel();

    bool swipeBegin(const uint8_t &fingers);
    bool swipeUpdate(const QPointF &delta);
    bool swipeEnd();
    bool swipeCancel();

    bool scroll(const qreal &delta, const Qt::Orientation &orientation, const qreal &inverted);

    void setSwipeDeltaMultiplier(const qreal &multiplier);
    void setScrollTimeout(const qreal &timeout);

private:
    qreal m_scrollTimeout = 100;
    QTimer m_scrollTimeoutTimer;
};

}