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

#include "effect.h"
#include "impl/kwinwindowinfoprovider.h"
#include "input/emitter.h"
#include "input/state.h"

#include <libinputactions/yaml_convert.h>

#include "effect/effecthandler.h"

#include <QDir>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(INPUTACTIONS_KWIN, "inputactions", QtWarningMsg)

const QString configFile = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/kwingestures.yml";

Effect::Effect()
{
    m_backend = new KWinInputBackend;
    libinputactions::InputBackend::setInstance(std::unique_ptr<KWinInputBackend>(m_backend));
    libinputactions::InputEmitter::setInstance(std::make_unique<KWinInputEmitter>());
    libinputactions::InputState::setInstance(std::make_unique<KWinInputState>());
    libinputactions::WindowInfoProvider::setImplementation(new KWinWindowInfoProvider);

#ifdef KWIN_6_2_OR_GREATER
    KWin::input()->installInputEventFilter(m_backend);
#else
    KWin::input()->prependInputEventFilter(m_backend);
#endif

    reconfigure(ReconfigureAll);

    if (!QFile::exists(configFile)) {
        QFile(configFile).open(QIODevice::WriteOnly);
    }
    m_configFileWatcher.addPath(configFile);
    m_configFileWatcher.addPath(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));
    connect(&m_configFileWatcher, &QFileSystemWatcher::directoryChanged, this, &Effect::slotConfigDirectoryChanged);
    connect(&m_configFileWatcher, &QFileSystemWatcher::fileChanged, this, &Effect::slotConfigFileChanged);
}

Effect::~Effect()
{
    if (KWin::input()) {
        KWin::input()->uninstallInputEventFilter(m_backend);
    }
}

void Effect::slotConfigFileChanged()
{
    if (!m_configFileWatcher.files().contains(configFile)) {
        m_configFileWatcher.addPath(configFile);
    }

    if (m_autoReload) {
        reconfigure(ReconfigureAll);
    }
}

void Effect::slotConfigDirectoryChanged()
{
    if (!m_configFileWatcher.files().contains(configFile) && QFile::exists(configFile)) {
        m_configFileWatcher.addPath(configFile);
        if (m_autoReload) {
            reconfigure(ReconfigureAll);
        }
    }
}

void Effect::reconfigure(ReconfigureFlags flags)
{
    try {
        const auto config = YAML::LoadFile(configFile.toStdString());
        m_autoReload = config["autoreload"].as<bool>(true);

        m_backend->setMouseTriggerHandler(std::make_unique<libinputactions::MouseTriggerHandler>());
        m_backend->setTouchpadTriggerHandler(std::make_unique<libinputactions::TouchpadTriggerHandler>());
        if (config["mouse"].IsDefined()) {
            m_backend->setMouseTriggerHandler(config["mouse"].as<std::unique_ptr<libinputactions::MouseTriggerHandler>>());
        }
        if (config["touchpad"].IsDefined()) {
            m_backend->setTouchpadTriggerHandler(config["touchpad"].as<std::unique_ptr<libinputactions::TouchpadTriggerHandler>>());
        }
    } catch (const YAML::Exception &e) {
        qCritical(INPUTACTIONS_KWIN).noquote() << QStringLiteral("Failed to load configuration: ") + QString::fromStdString(e.msg)
                + " (line " + QString::number(e.mark.line) + ", column " + QString::number(e.mark.column) + ")";
    }
}