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

#include "input/backend.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QObject>

class DBusInterface : public QObject
{
    Q_OBJECT

public:
    /**
     * Registers the interface.
     */
    DBusInterface();
    /**
     * Unregisters the interface.
     */
    ~DBusInterface() override;

public slots:
    Q_NOREPLY void recordStroke(const QDBusMessage &message);

private:
    QDBusConnection m_bus = QDBusConnection::sessionBus();
};