#include "libgestures/gestures/gesturerecognizer.h"
#include <QTest>

namespace libgestures
{

class TestGestureRecognizer : public QObject
{
    Q_OBJECT
private slots:
    void init();

    void gestureBegin_calledTwice_hasOneActiveGesture();
    void gestureBegin_gestureConditionsNotSatisfied_hasNoActiveGestures();
    void gestureBegin_twoGesturesWithSatisfiedConditions_hasTwoActiveGestures();

//    void gestureUpdate_activeGestureAndWrongDirection_gestureCancelledSignalEmittedAndReturnsFalse();
//    void gestureUpdate_activeGesture_gestureUpdateSignalEmittedExactlyOneTimeAndDeltaMatchesAndGestureNotEndedPrematurely();
//    void gestureUpdate_twoActiveGesturesAndOneEndsPrematurely_endedPrematurelySetToTrueAndOnlyOneGestureUpdated();

    void gestureCancel_twoActiveGestures_gestureCancelledSignalEmittedForAllGesturesAndActiveHoldGesturesCleared();

    void gestureEnd_noActiveGestures_returnsFalse();
    void gestureEnd_activeGesture_gestureEndedSignalEmittedAndActiveHoldGesturesClearedAndReturnsTrue();

    void pinchGestureUpdate_directions_data();
    void pinchGestureUpdate_directions();

    void swipeGestureUpdate_directions_data();
    void swipeGestureUpdate_directions();
private:
    void gestureUpdate_directions(
        std::shared_ptr<Gesture> gesture,
        std::function<void()> gestureBegin,
        std::function<bool()> gestureUpdate,
        const bool &correct
    );

    std::shared_ptr<GestureRecognizer> m_gestureRecognizer;

    std::shared_ptr<HoldGesture> m_hold2;
    std::shared_ptr<HoldGesture> m_hold2To3;
    std::shared_ptr<HoldGesture> m_hold3To4;
    std::shared_ptr<HoldGesture> m_hold2_2actions_trigger1only;
};

} // namespace libgestures