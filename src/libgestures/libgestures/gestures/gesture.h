#pragma once

#include "libgestures/actions/action.h"
#include "libgestures/input.h"

#include <QRegularExpression>

#include <set>
#include <vector>

namespace libgestures
{

enum class Edge {
    None = 0,
    Left = 1u << 0,
    Right = 1u << 1,
    Top = 1u << 2,
    Bottom = 1u << 3
};
Q_DECLARE_FLAGS(Edges, Edge)
Q_DECLARE_OPERATORS_FOR_FLAGS(Edges)

enum class GestureSpeed {
    Any,
    Slow,
    Fast
};

struct GestureBeginEvent
{
    uint8_t fingers = 1;
    Qt::KeyboardModifiers keyboardModifiers = Input::implementation()->keyboardModifiers();
    Qt::MouseButtons mouseButtons = Input::implementation()->mouseButtons();
    Edges edges = Input::implementation()->mouseScreenEdges(10);
};

/**
 * Gestures are handled by GestureHandler, which notifies them of update, end and cancellation events.
 *
 * Gesture lifecycle:
 *   - activate - The gesture is added to the list of active gestures in GestureHandler if the begin conditions were
 *     satisfied and will now receive update events.
 *   - start - Gestures are started during update events. Handled by Gesture. Threshold may delay this event.
 *   - update
 *   - end
 *   - cancel - The gesture's update conditions were not satisfied. May occur even if the gesture has not started.
 *
 * All above events other than activate must be forwarded to all actions.
 */
class Gesture
{
public:
    Gesture() = default;
    virtual ~Gesture() = default;

    void addAction(const std::shared_ptr<GestureAction> &action);
    void addCondition(const std::shared_ptr<const Condition> &condition);

    /**
     * @returns Whether the gesture can be activated.
     */
    bool satisfiesBeginConditions(const GestureBeginEvent &data) const;
    bool blocksMouseButton(const Qt::MouseButton &button) const;

    /**
     * Updates the gesture. Will start it immediately or when the threshold is reached.
     * @param deltaPointMultiplied Only used for swipe drag gestures. It sucks.
     * @return Whether the gesture is still active.
     */
    bool update(const qreal &delta, const QPointF &deltaPointMultiplied = QPointF());
    /**
     * Ends the gestures and executes end actions. Does nothing if the gesture hasn't started.
     */
    void end();
    /**
     * Cancels the gestures and executes cancel actions. Does nothing if the gesture hasn't started.
     */
    void cancel();

    const GestureSpeed &speed() const;
    const std::optional<std::set<Edges>> &edges() const;
    const std::optional<Qt::KeyboardModifiers> &keyboardModifiers() const;
    const std::optional<Qt::MouseButtons> &mouseButtons() const;

    void setSpeed(const GestureSpeed &speed);
    void setThresholds(const qreal &minimum, const qreal &maximum);
    void setFingers(const uint8_t &minimum, const uint8_t &maximum);

    /**
     * @param edges
     */
    void setEdges(const std::optional<std::set<Edges>> &edges);

    /**
     * @param modifiers std::nullopt - ignore modifiers, Qt::KeyboardModifier::NoModifier - no modifiers, anything
     * else - all specified modifiers must be active
     */
    void setKeyboardModifiers(const std::optional<Qt::KeyboardModifiers> &modifiers);

    /**
     * @param modifiers std::nullopt - ignore buttons, Qt::MouseButton::NoButton - no buttons, anything
     * else - all specified buttons must be pressed
     */
    void setMouseButtons(const std::optional<Qt::MouseButtons> &buttons);

protected:
    /**
     * @return Whether the gesture can be updated.
     */
    bool satisfiesUpdateConditions(const GestureSpeed &speed) const;

    void setFingerCountIsRelevant(const bool &relevant);

private:
    /**
     * @return Whether the accumulated delta fits within the specified range.
     */
    bool thresholdReached() const;

    uint8_t m_minimumFingers = 0;
    uint8_t m_maximumFingers = 0;
    bool m_fingerCountIsRelevant = true;

    qreal m_minimumThreshold = 0;
    qreal m_maximumThreshold = 0;
    GestureSpeed m_speed = GestureSpeed::Any;

    std::optional<std::set<Edges>> m_edges;
    std::optional<Qt::KeyboardModifiers> m_keyboardModifiers;
    std::optional<Qt::MouseButtons> m_mouseButtons;

    std::vector<std::shared_ptr<const Condition>> m_conditions;

    std::vector<std::shared_ptr<GestureAction>> m_actions;

    qreal m_absoluteAccumulatedDelta = 0;
    bool m_hasStarted = false;
};

}