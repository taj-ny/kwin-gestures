#include "gesture.h"

#include "libgestures/input.h"
#include "libgestures/logging.h"

namespace libgestures
{

void Gesture::cancel()
{
    qCDebug(LIBGESTURES_GESTURE).noquote() << QString("Gesture cancelled (name: %1)").arg(m_name);
    if (!m_hasStarted) {
        m_absoluteAccumulatedDelta = 0;
        m_thresholdReached = false;
        return;
    }
    m_hasStarted = false;

    for (const auto &action : m_actions) {
        action->gestureCancelled(m_thresholdReached);
        if (m_thresholdReached && action->blocksOtherActions()) {
            break;
        }
    }
    m_absoluteAccumulatedDelta = 0;
    m_thresholdReached = false;
}

void Gesture::end()
{
    qCDebug(LIBGESTURES_GESTURE).noquote() << QString("Gesture ended (name: %1)").arg(m_name);
    if (!m_hasStarted) {
        m_absoluteAccumulatedDelta = 0;
        m_thresholdReached = false;
        return;
    }
    m_hasStarted = false;

    for (const auto &action : m_actions) {
        action->gestureEnded(m_thresholdReached);
        if (m_thresholdReached && action->blocksOtherActions()) {
            break;
        }
    }
    m_absoluteAccumulatedDelta = 0;
    m_thresholdReached = false;
}

bool Gesture::update(const qreal &delta, const QPointF &deltaPointMultiplied)
{
    m_absoluteAccumulatedDelta += std::abs(delta);
    m_thresholdReached = thresholdReached();
    if (!m_thresholdReached) {
        qCDebug(LIBGESTURES_GESTURE).noquote()
            << QString("Threshold not reached (name: %1, current: %2, min: %3, max: %4")
                .arg(m_name, QString::number(m_absoluteAccumulatedDelta), QString::number(m_threshold.min()), QString::number(m_threshold.max()));
        return true;
    }

    qCDebug(LIBGESTURES_GESTURE).noquote() << QString("Gesture updated (name: %1, delta: %2)").arg(m_name, QString::number(delta));

    if (!m_hasStarted) {
        qCDebug(LIBGESTURES_GESTURE).noquote() << QString("Gesture started (name: %1)").arg(m_name);
        m_hasStarted = true;
        for (const auto &action : m_actions) {
            action->gestureStarted();
            if (action->blocksOtherActions()) {
                break;
            }
        }
    }

    for (const auto &action : m_actions) {
        action->gestureUpdated(delta, deltaPointMultiplied);
        if (action->blocksOtherActions()) {
            end();
            return false;
        }
    }

    return true;
}

bool Gesture::satisfiesBeginConditions(const GestureBeginEvent &data) const
{
    if ((m_fingerCountIsRelevant && data.fingers && !m_fingers.contains(*data.fingers))
        || (m_keyboardModifiers && data.keyboardModifiers && *m_keyboardModifiers != data.keyboardModifiers)
        || (m_mouseButtons && data.mouseButtons && *m_mouseButtons != data.mouseButtons)
        || (m_beginPositions && data.position && std::find_if(m_beginPositions->begin(), m_beginPositions->end(), [&data](const auto &position) {
            return position.contains(*data.position);
        }) == m_beginPositions->end())) {
        return false;
    }

    const bool satisfiesConditions = std::find_if(m_conditions.begin(), m_conditions.end(), [](const std::shared_ptr<const Condition> &condition) {
        return condition->isSatisfied();
    }) != m_conditions.end();
    if (!m_conditions.empty() && !satisfiesConditions) {
        return false;
    }

    const bool actionSatisfiesConditions = std::find_if(m_actions.begin(), m_actions.end(), [](const std::shared_ptr<const GestureAction> &triggerAction) {
        return triggerAction->satisfiesConditions();
    }) != m_actions.end();
    return m_actions.empty() || actionSatisfiesConditions;
}

bool Gesture::satisfiesEndConditions(const GestureEndEvent &event) const
{
    if (m_endPositions && event.position && std::find_if(m_endPositions->begin(), m_endPositions->end(), [&event](const auto &position) {
        return position.contains(*event.position);
    }) == m_endPositions->end()) {
        return false;
    }

    return true;
}

bool Gesture::shouldCancelOtherGestures(const bool &end)
{
    if (!m_thresholdReached) {
        return false;
    }

    return std::find_if(m_actions.begin(), m_actions.end(), [&end](const std::shared_ptr<const GestureAction> &action) {
        return end
            ? (action->on() == On::End && action->canExecute())
            : (action->executed() || (action->on() == On::Update && action->canExecute()));
    }) != m_actions.end();
}

bool Gesture::satisfiesUpdateConditions(const GestureSpeed &speed, const uint32_t &direction) const
{
    return m_speed == GestureSpeed::Any || m_speed == speed;
}

void Gesture::addAction(const std::shared_ptr<GestureAction> &action)
{
    m_actions.push_back(action);
}

void Gesture::addCondition(const std::shared_ptr<const Condition> &condition)
{
    m_conditions.push_back(condition);
}

void Gesture::setThreshold(const Range<qreal> &threshold)
{
    m_threshold = threshold;
}

void Gesture::setFingers(const Range<uint8_t> &fingers)
{
    m_fingers = fingers;
}

void Gesture::setFingerCountIsRelevant(const bool &relevant)
{
    m_fingerCountIsRelevant = relevant;
}

void Gesture::setBeginPositions(const std::optional<std::vector<Range<QPointF>>> &positions)
{
    m_beginPositions = positions;
}

void Gesture::setEndPositions(const std::optional<std::vector<Range<QPointF>>> &positions)
{
    m_endPositions = positions;
}

void Gesture::setKeyboardModifiers(const std::optional<Qt::KeyboardModifiers> &modifiers)
{
    m_keyboardModifiers = modifiers;
}

void Gesture::setMouseButtons(const std::optional<Qt::MouseButtons> &buttons)
{
    m_mouseButtons = buttons;
}

bool Gesture::thresholdReached() const
{
    return (m_threshold.min() == 0 || m_absoluteAccumulatedDelta >= m_threshold.min())
        && (m_threshold.max() == 0 || m_absoluteAccumulatedDelta <= m_threshold.max());
}

void Gesture::setSpeed(const libgestures::GestureSpeed &speed)
{
    m_speed = speed;
}

const GestureSpeed &Gesture::speed() const
{
    return m_speed;
}

const std::optional<Qt::KeyboardModifiers> &Gesture::keyboardModifiers() const
{
    return m_keyboardModifiers;
}

const std::optional<Qt::MouseButtons> &Gesture::mouseButtons() const
{
    return m_mouseButtons;
}

GestureType Gesture::type() const
{
    return GestureType(0);
}

const QString &Gesture::name() const
{
    return m_name;
}

void Gesture::setName(const QString &name)
{
    m_name = name;
}

}