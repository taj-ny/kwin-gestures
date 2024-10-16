#pragma once

#include "gestures/gesturerecognizer.h"

class TestGestureRecognizerShared
{
public:
    static void gestureBegin_calledTwice_hasOneActiveGesture
    (
        std::shared_ptr<GestureRecognizer> gestureRecognizer,
        std::shared_ptr<Gesture> gesture,
        std::function<void()> gestureBegin,
        std::function<QList<std::shared_ptr<Gesture>>(void)> activeGestures
    );
    static void gestureBegin_gestureConditionsNotSatisfied_hasNoActiveGestures
    (
        std::shared_ptr<GestureRecognizer> gestureRecognizer,
        std::shared_ptr<Gesture> gesture,
        std::function<void()> gestureBegin,
        std::function<QList<std::shared_ptr<Gesture>>(void)> activeGestures
    );
    static void gestureBegin_twoGesturesWithSatisfiedConditions_hasTwoActiveGestures
    (
        std::shared_ptr<GestureRecognizer> gestureRecognizer,
        std::vector<std::shared_ptr<Gesture>> gestures,
        std::function<void()> gestureBegin,
        std::function<QList<std::shared_ptr<Gesture>>(void)> activeGestures
    );

    template <typename TDelta>
    static void gestureUpdate_activeGesture_gestureUpdateSignalEmittedExactlyOneTimeAndDeltaMatchesAndGestureNotEndedPrematurely
    (
        std::shared_ptr<GestureRecognizer> gestureRecognizer,
        std::shared_ptr<Gesture> gesture,
        std::function<void()> gestureBegin,
        std::function<void(const TDelta&, bool&)> gestureUpdate,
        const TDelta &delta
    );
    static void gestureUpdate_twoActiveGesturesAndOneEndsPrematurely_endedPrematurelySetToTrueAndOnlyOneGestureUpdated
    (
        std::shared_ptr<GestureRecognizer> gestureRecognizer,
        std::shared_ptr<Gesture> gesture1,
        std::shared_ptr<Gesture> gesture2,
        std::function<void()> gestureBegin,
        std::function<void(bool&)> gestureUpdate
    );

    static void gestureCancelled_twoActiveGestures_gestureCancelledSignalEmittedForAllGesturesAndActiveGesturesCleared
    (
        std::shared_ptr<GestureRecognizer> gestureRecognizer,
        std::shared_ptr<Gesture> gesture1,
        std::shared_ptr<Gesture> gesture2,
        std::function<void()> gestureBegin,
        std::function<void()> gestureCancel,
        std::function<QList<std::shared_ptr<Gesture>>(void)> activeGestures
    );

    static void gestureEnd_activeGesture_gestureEndedSignalEmittedAndActiveHoldGesturesClearedAndReturnsTrue
    (
        std::shared_ptr<GestureRecognizer> gestureRecognizer,
        std::shared_ptr<Gesture> gesture,
        std::function<void()> gestureBegin,
        std::function<bool()> gestureEnd,
        std::function<QList<std::shared_ptr<Gesture>>(void)> activeGestures
    );
};