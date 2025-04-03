#include "handler.h"

#include "libgestures/logging.h"
#include "libgestures/input.h"

#include "libgestures/yaml_convert.h"

namespace libgestures
{

bool GestureHandler::gestureEndOrCancel(const GestureTypes &types, const bool &end)
{
    if (!hasActiveGestures(types)) {
        return false;
    }

    if (end) {
        qCDebug(LIBGESTURES_GESTURE_HANDLER).nospace() << "Ending gestures (types: " << types << ")";
    } else {
        qCDebug(LIBGESTURES_GESTURE_HANDLER).nospace() << "Cancelling gestures (types: " << types << ")";
    }

    auto active = activeGestures(types);
    auto hadActiveGestures = !active.empty();
    GestureEndEvent event;

    if (types & GestureType::Press && m_pressTimer.isActive()) {
        qCDebug(LIBGESTURES_GESTURE_HANDLER) << "Stopping hold timer";
        m_pressTimer.stop();
    }
    if (end && types & GestureType::Stroke && !m_stroke.empty()) {
        const Stroke stroke(m_stroke);
        qCDebug(LIBGESTURES_GESTURE_HANDLER).noquote()
            << QString("Stroke constructed (points: %1, deltas: %2)").arg(QString::number(stroke.points().size()), QString::number(m_stroke.size()));

        Gesture *bestGesture = nullptr;
        double bestScore = 0;
        for (const auto &gesture : activeGestures(GestureType::Stroke)) {
            if (!gesture->satisfiesEndConditions(event)) {
                continue;
            }

            for (const auto &gestureStroke : static_cast<StrokeGesture *>(gesture)->strokes()) {
                const auto score = stroke.compare(gestureStroke);
                if (score > bestScore && score > Stroke::min_matching_score()) {
                    bestGesture = gesture;
                    bestScore = score;
                }
            }
        }
        qCDebug(LIBGESTURES_GESTURE_HANDLER).noquote()
            << QString("Stroke compared (bestScore: %2)").arg(QString::number(bestScore));

        if (bestGesture) {
            gestureCancel(bestGesture);
            bestGesture->end();
        }
        gestureCancel(GestureType::Stroke); // TODO Double cancellation
    }

    for (auto it = m_activeGestures.begin(); it != m_activeGestures.end();) {
        auto gesture = *it;
        if (!(types & gesture->type())) {
            it++;
            continue;
        }

        it = m_activeGestures.erase(it);
        if (end && gesture->satisfiesEndConditions(event)) {
            if (gesture->shouldCancelOtherGestures(true)) {
                gestureCancel(gesture);
                gesture->end();
                break;
            }

            gesture->end();
            continue;
        }

        gesture->cancel();
    }

    return hadActiveGestures;
}
=

}