#include <QSignalSpy>
#include "test_action.h"

namespace libgestures
{

void TestAction::init()
{
    m_action = std::make_shared<GestureAction>();
}

void TestAction::satisfiesConditions_data()
{
    QTest::addColumn<std::vector<std::shared_ptr<Condition>>>("conditions");
    QTest::addColumn<bool>("result");

    QTest::newRow("none") << (std::vector<std::shared_ptr<Condition>>()) << true;
    QTest::newRow("one satisfied") << std::vector<std::shared_ptr<Condition>> { m_satisfiedCondition } << true;
    QTest::newRow("one unsatisfied") << std::vector<std::shared_ptr<Condition>> { m_unsatisfiedCondition } << false;
    QTest::newRow("one unsatisfied one satisfied") << std::vector<std::shared_ptr<Condition>> { m_unsatisfiedCondition, m_satisfiedCondition } << true;
}

void TestAction::satisfiesConditions()
{
    QFETCH(std::vector<std::shared_ptr<Condition>>, conditions);
    QFETCH(bool, result);

    for (auto &condition : conditions) {
        m_action->addCondition(condition);
    }

    QCOMPARE(m_action->satisfiesConditions(), result);
}

void TestAction::canExecute_nonRepeatingNotExecuted_returnsTrue()
{
    QVERIFY(m_action->canExecute());
}

void TestAction::canExecute_repeatingNotExecuted_returnsTrue()
{
    m_action->setRepeatInterval(1);
    QVERIFY(m_action->canExecute());
}

void TestAction::canExecute_nonRepeatingExecutedOnce_returnsFalse()
{
    m_action->tryExecute();
    QVERIFY(!m_action->canExecute());
}

void TestAction::canExecute_repeatingExecutedOnce_returnsTrue()
{
    m_action->setRepeatInterval(1);
    m_action->tryExecute();
    QVERIFY(m_action->canExecute());
}

void TestAction::canExecute_nonRepeatingExecutedOnceAndEnded_returnsTrue()
{
    m_action->tryExecute();
    Q_EMIT m_action->gestureEnded();
    QVERIFY(m_action->canExecute());
}

void TestAction::canExecute_nonRepeatingExecutedOnceAndCancelled_returnsTrue()
{
    m_action->tryExecute();
    Q_EMIT m_action->gestureCancelled();
    QVERIFY(m_action->canExecute());
}

void TestAction::onGestureUpdated_notRepeating_doesntExecuteAction()
{
    const QSignalSpy spy(m_action.get(), &GestureAction::executed);

    Q_EMIT m_action->gestureUpdated(1);

    QCOMPARE(spy.count(), 0);
}

void TestAction::onGestureUpdated_repeating_data()
{
    QTest::addColumn<int>("interval");
    QTest::addColumn<int>("delta1");
    QTest::addColumn<int>("delta2");
    QTest::addColumn<int>("actionExecutions");

    QTest::newRow("interval equal to delta") << 10 << 10 << 0 << 1;
    QTest::newRow("interval greater than delta") << 10 << 9 << 0 << 0;
    QTest::newRow("interval lesser than delta") << 10 << 11 << 0 << 1;
    QTest::newRow("negative interval positive delta") << -10 << 10 << 0 << 0;
    QTest::newRow("positive interval negative delta") << 10 << -10 << 0 << 0;
    QTest::newRow("multiple executions") << 10 << 55 << 0 << 5;
    QTest::newRow("two deltas") << 10 << 5 << 5 << 1;
    QTest::newRow("direction changed 1") << 10 << 20 << -40 << 2;
    QTest::newRow("direction changed 2") << -10 << -40 << 80 << 4;
}

void TestAction::onGestureUpdated_repeating()
{
    QFETCH(int, interval);
    QFETCH(int, delta1);
    QFETCH(int, delta2);
    QFETCH(int, actionExecutions);

    m_action->setRepeatInterval(interval);
    const QSignalSpy spy(m_action.get(), &GestureAction::executed);

    Q_EMIT m_action->gestureUpdated(delta1);
    Q_EMIT m_action->gestureUpdated(delta2);

    QCOMPARE(spy.count(), actionExecutions);
}

} // namespace libgestures

QTEST_MAIN(libgestures::TestAction)
#include "test_action.moc"