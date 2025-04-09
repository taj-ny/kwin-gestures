#include <libinputactions/gestures/gesturerecognizer.h>
#include <QTest>

namespace libinputactions
{

class TestGestureRecognizer : public QObject
{
    Q_OBJECT
private slots:
    void init();

    void gestureBegin_calledTwice_hasOneActiveGesture();
    void gestureBegin_gestureConditionsNotSatisfied_hasNoActiveGestures();
    void gestureBegin_twoGesturesWithSatisfiedConditions_hasTwoActiveGestures();

    void gestureCancel_twoActiveGestures_gestureCancelledSignalEmittedForAllGesturesAndActiveHoldGesturesCleared();

    void gestureEnd_noActiveGestures_returnsFalse();
    void gestureEnd_activeGesture_gestureEndedSignalEmittedAndActiveHoldGesturesClearedAndReturnsTrue();

    void holdGestureUpdate_twoActiveGesturesAndOneEndsPrematurely_endedPrematurelySetToTrueAndOnlyOneGestureUpdatedAndReturnsTrue();

    void pinchGestureUpdate_directions_data();
    void pinchGestureUpdate_directions();
    void pinchGestureUpdate_twoActiveGesturesAndOneEndsPrematurely_endedPrematurelySetToTrueAndOnlyOneGestureUpdatedAndReturnsTrue();

    void swipeGestureUpdate_directions_data();
    void swipeGestureUpdate_directions();
    void swipeGestureUpdate_twoActiveGesturesAndOneEndsPrematurely_endedPrematurelySetToTrueAndOnlyOneGestureUpdatedAndReturnsTrue();
private:
    void gestureUpdate_directions(
        std::shared_ptr<Gesture> gesture,
        std::function<void()> gestureBegin,
        std::function<bool()> gestureUpdate,
        const bool &correct
    );

    void gestureUpdate_twoActiveGesturesAndOneEndsPrematurely_endedPrematurelySetToTrueAndOnlyOneGestureUpdatedAndReturnsTrue(
        std::shared_ptr<Gesture> gesture1,
        std::shared_ptr<Gesture> gesture2,
        std::function<void()> gestureBegin,
        std::function<bool(bool&)> gestureUpdate
    );

    std::shared_ptr<GestureRecognizer> m_gestureRecognizer;

    std::shared_ptr<HoldGesture> m_hold2;
    std::shared_ptr<HoldGesture> m_hold2To3;
    std::shared_ptr<HoldGesture> m_hold3To4;
};

}