#include "condition.h"
#include "test_condition.h"

TestCondition::TestCondition()
    : m_noActiveWindow(std::make_shared<MockWindowDataProvider>(std::nullopt)),
      m_normalWindow(std::make_shared<MockWindowDataProvider>(std::make_optional<WindowData>("Firefox", "firefox", "firefox", WindowState::Unimportant))),
      m_maximizedWindow(std::make_shared<MockWindowDataProvider>(std::make_optional<WindowData>("Firefox", "firefox", "firefox", WindowState::Maximized))),
      m_fullscreenWindow(std::make_shared<MockWindowDataProvider>(std::make_optional<WindowData>("Firefox", "firefox", "firefox", WindowState::Maximized)))
{
}

void TestCondition::isSatisfied_noActiveWindow_returnsFalse()
{
    const auto condition = std::make_shared<Condition>(m_noActiveWindow, false, std::nullopt, std::nullopt);

    QVERIFY(!condition->isSatisfied());
}

void TestCondition::isSatisfied_negatedAndNoActiveWindow_returnsFalse()
{
    const auto condition = std::make_shared<Condition>(m_noActiveWindow, true, std::nullopt, std::nullopt);

    QVERIFY(!condition->isSatisfied());
}

void TestCondition::isSatisfied_noSubConditions_returnsTrue()
{
    const auto condition = std::make_shared<Condition>(m_normalWindow, false, std::nullopt, std::nullopt);

    QVERIFY(condition->isSatisfied());
}

void TestCondition::isSatisfied_negatedAndNoSubConditions_returnsFalse()
{
    const auto condition = std::make_shared<Condition>(m_normalWindow, true, std::nullopt, std::nullopt);

    QVERIFY(!condition->isSatisfied());
}

void TestCondition::isSatisfied_windowClassRegexMatchesResourceClass_returnsTrue()
{
    const auto window = std::make_shared<MockWindowDataProvider>(std::make_optional<WindowData>("Firefox", "firefox", "", WindowState::Unimportant));
    const auto condition = std::make_shared<Condition>(window, false, QRegularExpression("firefox"), std::nullopt);

    QVERIFY(condition->isSatisfied());
}

void TestCondition::isSatisfied_windowClassRegexMatchesResourceName_returnsTrue()
{
    const auto window = std::make_shared<MockWindowDataProvider>(std::make_optional<WindowData>("Firefox", "", "firefox", WindowState::Unimportant));
    const auto condition = std::make_shared<Condition>(window, false, QRegularExpression("firefox"), std::nullopt);

    QVERIFY(condition->isSatisfied());
}

void TestCondition::isSatisfied_windowClassRegexDoesntMatch_returnsFalse()
{
    const auto condition = std::make_shared<Condition>(m_normalWindow, false, QRegularExpression("firefoxx"), std::nullopt);

    QVERIFY(!condition->isSatisfied());
}

void TestCondition::isSatisfied_negatedAndWindowClassRegexMatches_returnsFalse()
{
    const auto window = std::make_shared<MockWindowDataProvider>(std::make_optional<WindowData>("Firefox", "firefox", "", WindowState::Unimportant));
    const auto condition = std::make_shared<Condition>(window, true, QRegularExpression("firefox"), std::nullopt);

    QVERIFY(!condition->isSatisfied());
}

void TestCondition::isSatisfied_negatedAndWindowClassRegexDoesntMatch_returnsTrue()
{
    const auto condition = std::make_shared<Condition>(m_normalWindow, true, QRegularExpression("firefoxx"), std::nullopt);

    QVERIFY(condition->isSatisfied());
}

void TestCondition::isSatisfied_windowStateDoesntMatch_returnsFalse()
{
    const auto condition = std::make_shared<Condition>(m_normalWindow, false, std::nullopt, WindowState::Maximized);

    QVERIFY(!condition->isSatisfied());
}

void TestCondition::isSatisfied_windowStateMatches_returnsTrue()
{
    const auto condition = std::make_shared<Condition>(m_maximizedWindow, false, std::nullopt, WindowState::Maximized);

    QVERIFY(condition->isSatisfied());
}

void TestCondition::isSatisfied_windowStateMatchesEither_returnsTrue()
{
    const auto condition = std::make_shared<Condition>(m_maximizedWindow, false, std::nullopt, WindowState::Fullscreen | WindowState::Maximized);

    QVERIFY(condition->isSatisfied());
}

void TestCondition::isSatisfied_negatedAndWindowStateDoesntMatch_returnsTrue()
{
    const auto condition = std::make_shared<Condition>(m_normalWindow, true, std::nullopt, WindowState::Maximized);

    QVERIFY(condition->isSatisfied());
}

void TestCondition::isSatisfied_negatedAndWindowStateMatches_returnsFalse()
{
    const auto condition = std::make_shared<Condition>(m_maximizedWindow, true, std::nullopt, WindowState::Maximized);

    QVERIFY(!condition->isSatisfied());
}

void TestCondition::isSatisfied_negatedAndWindowStateMatchesEither_returnsFalse()
{
    const auto condition = std::make_shared<Condition>(m_maximizedWindow, true, std::nullopt, WindowState::Fullscreen | WindowState::Maximized);

    QVERIFY(!condition->isSatisfied());
}

void TestCondition::isSatisfied_windowClassMatchesAndStateDoesnt_returnsFalse()
{
    const auto condition = std::make_shared<Condition>(m_normalWindow, false, QRegularExpression("firefox"), WindowState::Maximized);

    QVERIFY(!condition->isSatisfied());
}

void TestCondition::isSatisfied_windowClassDoesntMatchAndStateDoes_returnsFalse()
{
    const auto condition = std::make_shared<Condition>(m_maximizedWindow, false, QRegularExpression("firefoxx"), WindowState::Maximized);

    QVERIFY(!condition->isSatisfied());
}

void TestCondition::isSatisfied_windowClassMatchesAndStateMatches_returnsTrue()
{
    const auto condition = std::make_shared<Condition>(m_maximizedWindow, false, QRegularExpression("firefox"), WindowState::Maximized);

    QVERIFY(condition->isSatisfied());
}

void TestCondition::isSatisfied_negatedAndWindowClassMatchesAndStateDoesnt_returnsTrue()
{
    const auto condition = std::make_shared<Condition>(m_normalWindow, true, QRegularExpression("firefox"), WindowState::Maximized);

    QVERIFY(condition->isSatisfied());
}

void TestCondition::isSatisfied_negatedAndWindowClassDoesntMatchAndStateDoes_returnsTrue()
{
    const auto condition = std::make_shared<Condition>(m_maximizedWindow, true, QRegularExpression("firefoxx"), WindowState::Maximized);

    QVERIFY(condition->isSatisfied());
}

void TestCondition::isSatisfied_negatedAndWindowClassMatchesAndStateMatches_returnsFalse()
{
    const auto condition = std::make_shared<Condition>(m_maximizedWindow, true, QRegularExpression("firefox"), WindowState::Maximized);

    QVERIFY(!condition->isSatisfied());
}

QTEST_MAIN(TestCondition)
#include "test_condition.moc"