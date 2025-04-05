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

#include "logging.h"

Q_LOGGING_CATEGORY(LIBGESTURES, "libgestures", QtWarningMsg)
Q_LOGGING_CATEGORY(LIBGESTURES_ACTION, "libgestures.action", QtWarningMsg)
Q_LOGGING_CATEGORY(LIBGESTURES_GESTURE, "libgestures.gesture", QtWarningMsg)
Q_LOGGING_CATEGORY(LIBGESTURES_GESTURE_HANDLER, "libgestures.gesture.handler", QtWarningMsg)