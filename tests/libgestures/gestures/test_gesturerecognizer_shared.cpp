#include "test_gesturerecognizer_shared.h"

#include <QSignalSpy>
#include <QTest>

namespace libgestures
{

template void TestGestureRecognizerShared::gestureUpdate_activeGesture_gestureUpdateSignalEmittedExactlyOneTimeAndDeltaMatchesAndGestureNotEndedPrematurely<qreal>
(
    std::shared_ptr<GestureRecognizer> gestureRecognizer,
    std::shared_ptr<Gesture> gesture,
    std::function<void()> gestureBegin,
    std::function<void(const qreal&, bool&)> gestureUpdate,
    const qreal &delta
);
template void TestGestureRecognizerShared::gestureUpdate_activeGesture_gestureUpdateSignalEmittedExactlyOneTimeAndDeltaMatchesAndGestureNotEndedPrematurely<QPointF>
(
    std::shared_ptr<GestureRecognizer> gestureRecognizer,
    std::shared_ptr<Gesture> gesture,
    std::function<void()> gestureBegin,
    std::function<void(const QPointF&, bool&)> gestureUpdate,
    const QPointF &delta
);

void TestGestureRecognizerShared::gestureBegin_calledTwice_hasOneActiveGesture
(
    std::shared_ptr<GestureRecognizer> gestureRecognizer,
    std::shared_ptr<Gesture> gesture,
    std::function<void()> gestureBegin,
    std::function<QList<std::shared_ptr<Gesture>>(void)> activeGestures
)
{
    gestureRecognizer->registerGesture(gesture);

    gestureBegin();
    gestureBegin();

    QCOMPARE(activeGestures().size(), 1);
}

void TestGestureRecognizerShared::gestureBegin_gestureConditionsNotSatisfied_hasNoActiveGestures
(
    std::shared_ptr<GestureRecognizer> gestureRecognizer,
    std::shared_ptr<Gesture> gesture,
    std::function<void()> gestureBegin,
    std::function<QList<std::shared_ptr<Gesture>>(void)> activeGestures
)
{
    gestureRecognizer->registerGesture(gesture);

    gestureBegin();

    QVERIFY(activeGestures().empty());
}

void TestGestureRecognizerShared::gestureBegin_twoGesturesWithSatisfiedConditions_hasTwoActiveGestures
(
    std::shared_ptr<GestureRecognizer> gestureRecognizer,
    std::vector<std::shared_ptr<Gesture>> gestures,
    std::function<void()> gestureBegin,
    std::function<QList<std::shared_ptr<Gesture>>(void)> activeGestures
)
{
    for (const auto gesture : gestures)
        gestureRecognizer->registerGesture(gesture);

    gestureBegin();

    QCOMPARE(activeGestures().size(), 2);
}

template <typename TDelta>
void TestGestureRecognizerShared::gestureUpdate_activeGesture_gestureUpdateSignalEmittedExactlyOneTimeAndDeltaMatchesAndGestureNotEndedPrematurely
(
    std::shared_ptr<GestureRecognizer> gestureRecognizer,
    std::shared_ptr<Gesture> gesture,
    std::function<void()> gestureBegin,
    std::function<void(const TDelta&,bool&)> gestureUpdate,
    const TDelta &delta
)
{
    QSignalSpy spy(gesture.get(), &Gesture::updated);
    gestureRecognizer->registerGesture(gesture);

    gestureBegin();
    auto endedPrematurely = false;
    gestureUpdate(delta, endedPrematurely);

    QCOMPARE(spy.count(), 1);
    const auto signal = spy.takeFirst();
    QCOMPARE(signal.at(0), delta);
    QVERIFY(!endedPrematurely);
}

void TestGestureRecognizerShared::gestureUpdate_twoActiveGesturesAndOneEndsPrematurely_endedPrematurelySetToTrueAndOnlyOneGestureUpdated
(
    std::shared_ptr<GestureRecognizer> gestureRecognizer,
    std::shared_ptr<Gesture> gesture1,
    std::shared_ptr<Gesture> gesture2,
    std::function<void()> gestureBegin,
    std::function<void(bool&)> gestureUpdate
)
{
    QSignalSpy spy1(gesture1.get(), &Gesture::updated);
    QSignalSpy spy2(gesture2.get(), &Gesture::updated);
    gestureRecognizer->registerGesture(gesture1);
    gestureRecognizer->registerGesture(gesture2);

    gestureBegin();
    auto endedPrematurely = false;
    gestureUpdate(endedPrematurely);

    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 0);
    QVERIFY(endedPrematurely);
}

void TestGestureRecognizerShared::gestureCancelled_twoActiveGestures_gestureCancelledSignalEmittedForAllGesturesAndActiveGesturesCleared
(
    std::shared_ptr<GestureRecognizer> gestureRecognizer,
    std::shared_ptr<Gesture> gesture1,
    std::shared_ptr<Gesture> gesture2,
    std::function<void()> gestureBegin,
    std::function<void()> gestureCancel,
    std::function<QList<std::shared_ptr<Gesture>>(void)> activeGestures
)
{
    QSignalSpy spy1(gesture1.get(), &Gesture::cancelled);
    QSignalSpy spy2(gesture2.get(), &Gesture::cancelled);
    gestureRecognizer->registerGesture(gesture1);
    gestureRecognizer->registerGesture(gesture2);

    gestureBegin();
    gestureCancel();

    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 1);
    QVERIFY(activeGestures().empty());
};

void TestGestureRecognizerShared::gestureEnd_activeGesture_gestureEndedSignalEmittedAndActiveHoldGesturesClearedAndReturnsTrue
(
    std::shared_ptr<GestureRecognizer> gestureRecognizer,
    std::shared_ptr<Gesture> gesture,
    std::function<void()> gestureBegin,
    std::function<bool()> gestureEnd,
    std::function<QList<std::shared_ptr<Gesture>>(void)> activeGestures
)
{
    QSignalSpy spy(gesture.get(), &Gesture::ended);
    gestureRecognizer->registerGesture(gesture);

    gestureBegin();
    const auto returnValue = gestureEnd();

    QCOMPARE(spy.count(), 1);
    QVERIFY(activeGestures().empty());
    QVERIFY(returnValue);
}

} // namespace libgestures