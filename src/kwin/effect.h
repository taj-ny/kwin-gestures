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

#include "dbusinterface.h"
#include "effect/effect.h"
#include "input/backend.h"

#include <QFileSystemWatcher>

class Effect : public KWin::Effect
{
public:
    Effect();
    ~Effect() override;

    static bool supported()
    {
        return true;
    };
    static bool enabledByDefault()
    {
        return false;
    };

    void reconfigure(ReconfigureFlags flags) override;

private slots:
    void slotConfigFileChanged();
    void slotConfigDirectoryChanged();

private:
    bool m_autoReload = true;
    KWinInputBackend *m_backend;
    DBusInterface m_dbusInterface;
    QFileSystemWatcher m_configFileWatcher;
};