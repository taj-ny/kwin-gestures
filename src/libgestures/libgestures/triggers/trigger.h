#pragma once

#include "libgestures/actions/action.h"
#include "libgestures/conditions/condition.h"
#include "libgestures/input.h"

#include <QLoggingCategory>
#include <QString>

Q_DECLARE_LOGGING_CATEGORY(LIBGESTURES_TRIGGER)

namespace libgestures
{
Q_NAMESPACE

enum class TriggerType : uint32_t {
    None = 0,
    Pinch = 1u << 0,
    Press = 1u << 1,
    Rotate = 1u << 2,
    Stroke = 1u << 3,
    Swipe = 1u << 4,
    Wheel = 1u << 5,

    PinchRotate = Pinch | Rotate,
    StrokeSwipe = Stroke | Swipe,

    All = UINT32_MAX
};
Q_ENUM_NS(TriggerType)
Q_DECLARE_FLAGS(TriggerTypes, TriggerType)
Q_DECLARE_OPERATORS_FOR_FLAGS(TriggerTypes)

class TriggerActivateEvent
{
public:
    std::optional<uint8_t> fingers;
    std::optional<Qt::KeyboardModifiers> keyboardModifiers;
    std::optional<Qt::MouseButtons> mouseButtons;
    std::optional<QPointF> position;
};
class TriggerUpdateEvent
{
public:
    TriggerUpdateEvent() = default;
    virtual ~TriggerUpdateEvent() = default;

    const qreal &delta() const;
    void setDelta(const qreal &delta);

private:
    qreal m_delta = 0;
};
struct TriggerEndEvent
{
    std::optional<QPointF> position;
};

/**
 * An input action that does not involve motion.
 *
 * Triggers are managed by a trigger handler.
 */
class Trigger
{
public:
    Trigger() = default;
    virtual ~Trigger() = default;

    void addAction(const std::unique_ptr<GestureAction> &action);

    /**
     * @return Whether conditions, fingers, keyboard modifiers, mouse buttons and begin positions are satisfied.
     */
    bool canActivate(const TriggerActivateEvent *event) const;

    /**
     * Called by the trigger handler before updating a trigger. If true is returned, that trigger will be cancelled.
     */
    virtual bool canUpdate(const TriggerUpdateEvent *event) const;
    virtual void update(const TriggerUpdateEvent *event);

    /**
     * Called by the trigger handler before ending a trigger. If true is returned, that trigger will be cancelled
     * instead of ended.
     *
     * @return Whether the trigger can be ended.
     */
    bool canEnd(const TriggerEndEvent *event) const;
    /**
     * Resets the trigger and informs all actions that it has ended.
     */
    void end();

    /**
     * Resets the trigger and informs all actions that it has been cancelled.
     */
    void cancel();

    /**
     * The trigger handler calls this method before ending a trigger. If true is returned, that trigger is ended and
     * all others are cancelled.
     *
     * @return Whether the trigger has an action that executes on trigger and can be executed.
     */
    bool overridesOtherTriggersOnEnd();
    /**
     * The trigger handler calls this method after updating a trigger. If true is returned for one, all other triggers
     * are cancelled.
     *
     * @return Whether the trigger has any action that has been executed or is an update action and can be executed.
     */
    bool overridesOtherTriggersOnUpdate();

    /**
     * Ignored unless set.
     */
    void addCondition(const std::shared_ptr<const Condition> &condition);
    /**
     * Ignored unless set. Does not apply to mouse triggers.
     *
     * @param fingers Range of fingers the trigger must be performed with.
     */
    void setFingers(const Range<qreal> &fingers);
    /**
     * Ignored unless set.
     *
     * Currently only supports mouse triggers, for which the cursor position relative to the screen it is currently
     * on is used.
     *
     * @param positions Exact rectangle(s) on the input device where the trigger must begin.
     */
    void setBeginPositions(const std::vector<Range<QPointF>> &positions);
    /**
     * @see setBeginPositions
     */
    void setEndPositions(const std::vector<Range<QPointF>> &positions);
    /**
     * Ignored unless set.
     *
     * @param threshold How far the gesture must progress in order to begin.
     */
    void setThreshold(const Range<qreal> &threshold);

    const std::optional<Qt::KeyboardModifiers> &keyboardModifiers() const;
    /**
     * Ignored unless set.
     *
     * @param modifiers Keyboard modifiers that must be pressed before and during the trigger.
     */
    void setKeyboardModifiers(const Qt::KeyboardModifiers &modifiers);

    const std::optional<Qt::MouseButtons> &mouseButtons() const;
    /**
     * Only applies to mouse triggers.
     *
     * @param buttons Mouse buttons that must be pressed before and during the trigger.
     */
    void setMouseButtons(const std::optional<Qt::MouseButtons> &buttons);

    const QString &name() const;
    /**
     * @param name Shown in debug logs.
     */
    void setName(const QString &name);

    const TriggerType &type() const;
    /**
     * Must be set.
     */
    void setType(const TriggerType &type);

protected:
    virtual void updateDerived(const TriggerUpdateEvent *event);

private:
    bool thresholdReached() const;

    void reset();

    QString m_name = "none";

    std::optional<std::shared_ptr<ConditionV2>> m_condition;
    Range<uint8_t> m_fingers = Range<uint8_t>(0, UINT8_MAX);
    std::optional<std::vector<Range<QPointF>>> m_beginPositions;
    std::optional<std::vector<Range<QPointF>>> m_endPositions;
    std::optional<Qt::KeyboardModifiers> m_keyboardModifiers;
    std::optional<Qt::MouseButtons> m_mouseButtons;

    std::optional<Range<qreal>> m_threshold;
    bool m_thresholdReached = false;
    qreal m_absoluteAccumulatedDelta = 0;

    std::vector<std::unique_ptr<GestureAction>> m_actions;
    bool m_started = false;
};

}