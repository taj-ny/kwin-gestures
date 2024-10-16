#include "test_gesturerecognizer_hold.h"
#include <QSignalSpy>

#include <memory>

void TestGestureRecognizerHold::init()
{
    m_gestureRecognizer = std::make_unique<GestureRecognizer>();
    m_hold2 = std::make_shared<HoldGesture>(false, 2, 2, false, 0);
    m_hold2To3 = std::make_shared<HoldGesture>(false, 2, 3, false, 0);
    m_hold3To4 = std::make_shared<HoldGesture>(false, 3, 4, false, 0);

    m_hold2_2actions_trigger1only = std::make_shared<HoldGesture>(true, 2, 2, true, 1);
    m_hold2_2actions_trigger1only->addAction(std::make_shared<GestureAction>(0));
    m_hold2_2actions_trigger1only->addAction(std::make_shared<GestureAction>(0));
}

void TestGestureRecognizerHold::holdGestureBegin_holdGestureConditionsNotSatisfied_hasNoActiveHoldGestures()
{
    m_gestureRecognizer->registerGesture(m_hold2);

    m_gestureRecognizer->holdGestureBegin(3);

    QVERIFY(m_gestureRecognizer->m_activeHoldGestures.empty());
}

void TestGestureRecognizerHold::holdGestureBegin_twoHoldGesturesWithSatisfiedConditions_hasTwoActiveHoldGestures()
{
    m_gestureRecognizer->registerGesture(m_hold2To3);
    m_gestureRecognizer->registerGesture(m_hold3To4);

    m_gestureRecognizer->holdGestureBegin(3);

    QCOMPARE(m_gestureRecognizer->m_activeHoldGestures.size(), 2);
}

void TestGestureRecognizerHold::holdGestureUpdate_activeGesture_gestureUpdateSignalEmittedExactlyOneTimeAndGestureNotEndedPrematurely()
{
    QSignalSpy spy(m_hold2.get(), &Gesture::updated);
    m_gestureRecognizer->registerGesture(m_hold2);

    m_gestureRecognizer->holdGestureBegin(2);
    auto endedPrematurely = false;
    const auto delta = 1;
    m_gestureRecognizer->holdGestureUpdate(delta, endedPrematurely);

    QCOMPARE(spy.count(), 1);
    const auto signal = spy.takeFirst();
    QCOMPARE(signal.at(0).toReal(), delta);
    QVERIFY(!endedPrematurely);
}

void TestGestureRecognizerHold::holdGestureUpdate_twoActiveGesturesAndOneEndsPrematurely_endedPrematurelySetToTrueAndOnlyOneGestureUpdated()
{
    QSignalSpy spy(m_hold2_2actions_trigger1only.get(), &Gesture::updated);
    m_gestureRecognizer->registerGesture(m_hold2_2actions_trigger1only);
    m_gestureRecognizer->registerGesture(m_hold2_2actions_trigger1only);

    m_gestureRecognizer->holdGestureBegin(2);
    auto endedPrematurely = false;
    m_gestureRecognizer->holdGestureUpdate(1, endedPrematurely);

    QCOMPARE(spy.count(), 1);
    QVERIFY(endedPrematurely);
}

void TestGestureRecognizerHold::holdGestureCancelled_twoActiveGestures_gestureCancelledSignalEmittedForAllGesturesAndActiveHoldGesturesCleared()
{
    QSignalSpy spy1(m_hold2To3.get(), &Gesture::cancelled);
    QSignalSpy spy2(m_hold3To4.get(), &Gesture::cancelled);
    m_gestureRecognizer->registerGesture(m_hold2To3);
    m_gestureRecognizer->registerGesture(m_hold3To4);

    m_gestureRecognizer->holdGestureBegin(3);
    m_gestureRecognizer->holdGestureCancelled();

    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 1);
    QVERIFY(m_gestureRecognizer->m_activeHoldGestures.empty());
}

void TestGestureRecognizerHold::holdGestureEnd_noActiveGestures_returnsFalse()
{
    QVERIFY(!m_gestureRecognizer->holdGestureEnd());
}

void TestGestureRecognizerHold::holdGestureEnd_activeGesture_gestureEndedSignalEmittedAndActiveHoldGesturesClearedAndReturnsTrue()
{
    QSignalSpy spy(m_hold2.get(), &Gesture::ended);
    m_gestureRecognizer->registerGesture(m_hold2);

    m_gestureRecognizer->holdGestureBegin(2);
    const auto returnValue = m_gestureRecognizer->holdGestureEnd();

    QCOMPARE(spy.count(), 1);
    QVERIFY(m_gestureRecognizer->m_activeHoldGestures.empty());
    QVERIFY(returnValue);
}

QTEST_MAIN(TestGestureRecognizerHold)
#include "test_gesturerecognizer_hold.moc"