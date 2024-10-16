#include "gestures/gesturerecognizer.h"
#include <QTest>

class TestGestureRecognizerHold : public QObject
{
    Q_OBJECT
private slots:
    void init();

    void holdGestureBegin_calledTwice_hasOneActiveGesture();
    void holdGestureBegin_gestureConditionsNotSatisfied_hasNoActiveGestures();
    void holdGestureBegin_twoGesturesWithSatisfiedConditions_hasTwoActiveGestures();

    void holdGestureUpdate_activeGesture_gestureUpdateSignalEmittedExactlyOneTimeAndDeltaMatchesAndGestureNotEndedPrematurely();
    void holdGestureUpdate_twoActiveGesturesAndOneEndsPrematurely_endedPrematurelySetToTrueAndOnlyOneGestureUpdated();

    void holdGestureCancelled_twoActiveGestures_gestureCancelledSignalEmittedForAllGesturesAndActiveHoldGesturesCleared();

    void holdGestureEnd_noActiveGestures_returnsFalse();
    void holdGestureEnd_activeGesture_gestureEndedSignalEmittedAndActiveHoldGesturesClearedAndReturnsTrue();
private:
    std::shared_ptr<GestureRecognizer> m_gestureRecognizer;

    std::shared_ptr<HoldGesture> m_hold2;
    std::shared_ptr<HoldGesture> m_hold2To3;
    std::shared_ptr<HoldGesture> m_hold3To4;
    std::shared_ptr<HoldGesture> m_hold2_2actions_trigger1only;
};
