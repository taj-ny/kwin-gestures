#pragma once

#include "condition.h"
#include "mockwindowdataprovider.h"
#include <QTest>

static const QString s_windowCaption = "Firefox";
static const QString s_windowClass = "firefox";

class TestCondition : public QObject
{
    Q_OBJECT
private slots:
    void init();

    void isSatisfied_noActiveWindow_returnsFalse();
    void isSatisfied_noSubConditions_returnsTrue();
    void isSatisfied_negatedAndNoSubConditions_returnsTrue();

    void isWindowClassRegexSubConditionSatisfied_subConditionNotSet_returnsTrue();
    void isWindowClassRegexSubConditionSatisfied_negatedAndSubConditionNotSet_returnsTrue();
    void isWindowClassRegexSubConditionSatisfied_data();
    void isWindowClassRegexSubConditionSatisfied();

    void isWindowStateSubConditionSatisfied_subConditionNotSet_returnsTrue();
    void isWindowStateSubConditionSatisfied_negatedAndSubConditionNotSet_returnsTrue();
    void isWindowStateSubConditionSatisfied_data();
    void isWindowStateSubConditionSatisfied();
private:
    const std::shared_ptr<MockWindowDataProvider> m_noActiveWindowProvider = std::make_shared<MockWindowDataProvider>(std::nullopt);
    const std::shared_ptr<MockWindowDataProvider> m_normalWindowProvider = std::make_shared<MockWindowDataProvider>(std::make_optional<WindowData>(s_windowCaption, s_windowClass, s_windowClass, WindowState::Unimportant));
    const WindowData m_normalWindow = m_normalWindowProvider->getDataForActiveWindow().value();

    std::shared_ptr<Condition> m_condition = std::make_shared<Condition>(m_normalWindowProvider);
};

