#include "test_gesturerecognizer_shared.h"
#include "test_gesturerecognizer_hold.h"
#include <QSignalSpy>

namespace libgestures
{

void TestGestureRecognizerHold::init()
{
    m_gestureRecognizer = std::make_shared<GestureRecognizer>();
    m_hold2 = std::make_shared<HoldGesture>(false, 2, 2, false, 0);
    m_hold2To3 = std::make_shared<HoldGesture>(false, 2, 3, false, 0);
    m_hold3To4 = std::make_shared<HoldGesture>(false, 3, 4, false, 0);

    m_hold2_2actions_trigger1only = std::make_shared<HoldGesture>(true, 2, 2, true, 1);
    m_hold2_2actions_trigger1only->addAction(std::make_shared<GestureAction>(0));
    m_hold2_2actions_trigger1only->addAction(std::make_shared<GestureAction>(0));
}

void TestGestureRecognizerHold::holdGestureBegin_calledTwice_hasOneActiveGesture()
{
    TestGestureRecognizerShared::gestureBegin_calledTwice_hasOneActiveGesture
    (
        m_gestureRecognizer,
        m_hold2,
        [this]()
        {
            m_gestureRecognizer->holdGestureBegin(2);
        },
        [this]()
        {
            return m_gestureRecognizer->m_activeHoldGestures;
        }
    );
}

void TestGestureRecognizerHold::holdGestureBegin_gestureConditionsNotSatisfied_hasNoActiveGestures()
{
    TestGestureRecognizerShared::gestureBegin_gestureConditionsNotSatisfied_hasNoActiveGestures
    (
        m_gestureRecognizer,
        m_hold2,
        [this]()
        {
            m_gestureRecognizer->holdGestureBegin(3);
        },
        [this]()
        {
            return m_gestureRecognizer->m_activeHoldGestures;
        }
    );
}

void TestGestureRecognizerHold::holdGestureBegin_twoGesturesWithSatisfiedConditions_hasTwoActiveGestures()
{
    TestGestureRecognizerShared::gestureBegin_twoGesturesWithSatisfiedConditions_hasTwoActiveGestures
    (
        m_gestureRecognizer,
        { m_hold2To3, m_hold3To4 },
        [this]()
        {
            m_gestureRecognizer->holdGestureBegin(3);
        },
        [this]()
        {
            return m_gestureRecognizer->m_activeHoldGestures;
        }
    );
}

void TestGestureRecognizerHold::holdGestureUpdate_activeGesture_gestureUpdateSignalEmittedExactlyOneTimeAndDeltaMatchesAndGestureNotEndedPrematurely()
{
    TestGestureRecognizerShared::gestureUpdate_activeGesture_gestureUpdateSignalEmittedExactlyOneTimeAndDeltaMatchesAndGestureNotEndedPrematurely<qreal>
    (
        m_gestureRecognizer,
        m_hold2,
        [this]() {
            m_gestureRecognizer->holdGestureBegin(2);
        },
        [this](const qreal & delta, bool &endedPrematurely) {
            m_gestureRecognizer->holdGestureUpdate(delta, endedPrematurely);
        },
        1
    );
}

void TestGestureRecognizerHold::holdGestureUpdate_twoActiveGesturesAndOneEndsPrematurely_endedPrematurelySetToTrueAndOnlyOneGestureUpdated()
{
    const auto delta = 1;
    TestGestureRecognizerShared::gestureUpdate_twoActiveGesturesAndOneEndsPrematurely_endedPrematurelySetToTrueAndOnlyOneGestureUpdated
    (
        m_gestureRecognizer,
        m_hold2_2actions_trigger1only,
        m_hold2,
        [this]()
        {
            m_gestureRecognizer->holdGestureBegin(2);
        },
        [this](bool &endedPrematurely)
        {
            m_gestureRecognizer->holdGestureUpdate(delta, endedPrematurely);
        }
    );
}

void TestGestureRecognizerHold::holdGestureCancelled_twoActiveGestures_gestureCancelledSignalEmittedForAllGesturesAndActiveHoldGesturesCleared()
{
    TestGestureRecognizerShared::gestureCancelled_twoActiveGestures_gestureCancelledSignalEmittedForAllGesturesAndActiveGesturesCleared
    (
        m_gestureRecognizer,
        m_hold2To3,
        m_hold3To4,
        [this]()
        {
            m_gestureRecognizer->holdGestureBegin(3);
        },
        [this]()
        {
            return m_gestureRecognizer->holdGestureCancelled();
        },
        [this]()
        {
            return m_gestureRecognizer->m_activeHoldGestures;
        }
    );
}

void TestGestureRecognizerHold::holdGestureEnd_noActiveGestures_returnsFalse()
{
    QVERIFY(!m_gestureRecognizer->holdGestureEnd());
}

void TestGestureRecognizerHold::holdGestureEnd_activeGesture_gestureEndedSignalEmittedAndActiveHoldGesturesClearedAndReturnsTrue()
{
    TestGestureRecognizerShared::gestureEnd_activeGesture_gestureEndedSignalEmittedAndActiveHoldGesturesClearedAndReturnsTrue
    (
        m_gestureRecognizer,
        m_hold2,
        [this]()
        {
            m_gestureRecognizer->holdGestureBegin(2);
        },
        [this]()
        {
            return m_gestureRecognizer->holdGestureEnd();
        },
        [this]()
        {
            return m_gestureRecognizer->m_activeHoldGestures;
        }
    );
}

} // namespace libgestures

QTEST_MAIN(libgestures::TestGestureRecognizerHold)
#include "test_gesturerecognizer_hold.moc"