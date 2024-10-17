#include "test_gesturerecognizer_shared.h"
#include "test_gesturerecognizer.h"
#include <QSignalSpy>

namespace libgestures
{

void TestGestureRecognizer::init()
{
    m_gestureRecognizer = std::make_shared<GestureRecognizer>();
    m_hold2 = std::make_shared<HoldGesture>();
    m_hold2->setFingers(2, 2);
    m_hold2To3 = std::make_shared<HoldGesture>();
    m_hold2To3->setFingers(2, 3);
    m_hold3To4 = std::make_shared<HoldGesture>();
    m_hold3To4->setFingers(3, 4);

    m_hold2_2actions_trigger1only = std::make_shared<HoldGesture>();
    m_hold2_2actions_trigger1only->setTriggerWhenThresholdReached(true);
    m_hold2_2actions_trigger1only->setFingers(2, 2);
    m_hold2_2actions_trigger1only->setTriggerOneActionOnly(true);
    m_hold2_2actions_trigger1only->setThreshold(1);
    m_hold2_2actions_trigger1only->addAction(std::make_shared<GestureAction>());
    m_hold2_2actions_trigger1only->addAction(std::make_shared<GestureAction>());
}

void TestGestureRecognizer::gestureBegin_calledTwice_hasOneActiveGesture()
{
    m_gestureRecognizer->registerGesture(m_hold2);

    m_gestureRecognizer->gestureBegin(2, m_gestureRecognizer->m_activeHoldGestures);
    m_gestureRecognizer->gestureBegin(2, m_gestureRecognizer->m_activeHoldGestures);

    QCOMPARE(m_gestureRecognizer->m_activeHoldGestures.size(), 1);
}

void TestGestureRecognizer::gestureBegin_gestureConditionsNotSatisfied_hasNoActiveGestures()
{
    m_gestureRecognizer->registerGesture(m_hold2);

    m_gestureRecognizer->gestureBegin(3, m_gestureRecognizer->m_activeHoldGestures);

    QVERIFY(m_gestureRecognizer->m_activeHoldGestures.empty());
}

void TestGestureRecognizer::gestureBegin_twoGesturesWithSatisfiedConditions_hasTwoActiveGestures()
{
    m_gestureRecognizer->registerGesture(m_hold2To3);
    m_gestureRecognizer->registerGesture(m_hold3To4);

    m_gestureRecognizer->gestureBegin(3, m_gestureRecognizer->m_activeHoldGestures);

    QCOMPARE(m_gestureRecognizer->m_activeHoldGestures.size(), 2);
}

void TestGestureRecognizer::gestureCancel_twoActiveGestures_gestureCancelledSignalEmittedForAllGesturesAndActiveHoldGesturesCleared()
{
    const QSignalSpy spy1(m_hold2To3.get(), &Gesture::cancelled);
    const QSignalSpy spy2(m_hold3To4.get(), &Gesture::cancelled);
    m_gestureRecognizer->registerGesture(m_hold2To3);
    m_gestureRecognizer->registerGesture(m_hold3To4);

    m_gestureRecognizer->gestureBegin(3, m_gestureRecognizer->m_activeHoldGestures);
    m_gestureRecognizer->gestureCancel(m_gestureRecognizer->m_activeHoldGestures);

    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 1);
    QVERIFY(m_gestureRecognizer->m_activeHoldGestures.empty());
}

void TestGestureRecognizer::gestureEnd_noActiveGestures_returnsFalse()
{
    QVERIFY(!m_gestureRecognizer->gestureEnd(m_gestureRecognizer->m_activeHoldGestures));
}

void TestGestureRecognizer::gestureEnd_activeGesture_gestureEndedSignalEmittedAndActiveHoldGesturesClearedAndReturnsTrue()
{
    const QSignalSpy spy(m_hold2.get(), &Gesture::ended);
    m_gestureRecognizer->registerGesture(m_hold2);

    m_gestureRecognizer->gestureBegin(2, m_gestureRecognizer->m_activeHoldGestures);
    const auto returnValue = m_gestureRecognizer->gestureEnd(m_gestureRecognizer->m_activeHoldGestures);

    QCOMPARE(spy.count(), 1);
    QVERIFY(m_gestureRecognizer->m_activeHoldGestures.empty());
    QVERIFY(returnValue);
}

void TestGestureRecognizer::pinchGestureUpdate_directions_data()
{
    QTest::addColumn<PinchDirection>("direction");
    QTest::addColumn<qreal>("delta");
    QTest::addColumn<bool>("correct");

    QTest::addRow("in in correct") << PinchDirection::Contracting << 0.9 << true;
    QTest::addRow("in out wrong") << PinchDirection::Contracting << 1.1 << false;
    QTest::addRow("out in wrong") << PinchDirection::Expanding << 0.9 << false;
    QTest::addRow("out out correct") << PinchDirection::Expanding << 1.1 << true;
    QTest::addRow("any in correct") << PinchDirection::Any << 0.9 << true;
    QTest::addRow("any out correct") << PinchDirection::Any << 1.1 << true;
}

void TestGestureRecognizer::pinchGestureUpdate_directions()
{
    QFETCH(PinchDirection, direction);
    QFETCH(qreal, delta);
    QFETCH(bool, correct);

    auto gesture = std::make_shared<PinchGesture>();
    gesture->setFingers(3, 3);
    gesture->setDirection(direction);

    gestureUpdate_directions(
        gesture,
        [this]()
        {
            m_gestureRecognizer->gestureBegin(3, m_gestureRecognizer->m_activePinchGestures);
        },
        [this, &delta]()
        {
            bool _ = false;
            return m_gestureRecognizer->pinchGestureUpdate(delta, 0, QPointF(), _);
        },
        correct
    );
}

void TestGestureRecognizer::swipeGestureUpdate_directions_data()
{
    QTest::addColumn<SwipeDirection>("direction");
    QTest::addColumn<QPointF>("delta");
    QTest::addColumn<bool>("correct");

    for (const auto &expectedDirection : std::vector<QString> {"left", "right", "up", "down", "leftright", "updown" })
    {
        for (const auto &direction : std::vector<QString> {"left", "right", "up", "down" })
        {
            SwipeDirection swipeDirection;
            if (expectedDirection == "left")
                swipeDirection = SwipeDirection::Left;
            else if (expectedDirection == "right")
                swipeDirection = SwipeDirection::Right;
            else if (expectedDirection == "up")
                swipeDirection = SwipeDirection::Up;
            else if (expectedDirection == "down")
                swipeDirection = SwipeDirection::Down;
            else if (expectedDirection == "leftright")
                swipeDirection = SwipeDirection::LeftRight;
            else if (expectedDirection == "updown")
                swipeDirection = SwipeDirection::UpDown;
            else
                Q_UNREACHABLE();

            QPointF delta;
            if (direction == "left")
                delta = QPointF(-10, 0);
            else if (direction == "right")
                delta = QPointF(10, 0);
            else if (direction == "up")
                delta = QPointF(0, -10);
            else if (direction == "down")
                delta = QPointF(0, 10);
            else
                Q_UNREACHABLE();

            bool correct = (expectedDirection == direction)
                || (swipeDirection == SwipeDirection::LeftRight && (direction == "left" || direction == "right"))
                || (swipeDirection == SwipeDirection::UpDown && (direction == "up" || direction == "down"));

            QTest::addRow("%s %s %s", expectedDirection.toStdString().c_str(), direction.toStdString().c_str(), correct ? "correct" : "wrong")
                << swipeDirection << delta << correct;
        }
    }
}

void TestGestureRecognizer::swipeGestureUpdate_directions()
{
    QFETCH(SwipeDirection, direction);
    QFETCH(QPointF, delta);
    QFETCH(bool, correct);

    auto gesture = std::make_shared<SwipeGesture>();
    gesture->setFingers(3, 3);
    gesture->setDirection(direction);

    gestureUpdate_directions(
        gesture,
        [this]()
        {
            m_gestureRecognizer->gestureBegin(3, m_gestureRecognizer->m_activeSwipeGestures);
        },
        [this, &delta]()
        {
            bool _ = false;
            return m_gestureRecognizer->swipeGestureUpdate(delta, _);
        },
        correct
    );
}

void TestGestureRecognizer::gestureUpdate_directions(
    std::shared_ptr<Gesture> gesture,
    std::function<void()> gestureBegin,
    std::function<bool()> gestureUpdate,
    const bool &correct
)
{
    const QSignalSpy cancelledSpy(gesture.get(), &Gesture::cancelled);
    const QSignalSpy updatedSpy(gesture.get(), &Gesture::updated);

    m_gestureRecognizer->registerGesture(gesture);

    gestureBegin();

    QCOMPARE(gestureUpdate(), correct);
    QCOMPARE(cancelledSpy.count(), correct ? 0 : 1);
    QCOMPARE(updatedSpy.count(), correct ? 1 : 0);
}

} // namespace libgestures

QTEST_MAIN(libgestures::TestGestureRecognizer)
#include "test_gesturerecognizer.moc"