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
                .arg(m_name, QString::number(m_absoluteAccumulatedDelta), QString::number(m_minimumThreshold), QString::number(m_maximumThreshold));
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
    if ((m_fingerCountIsRelevant && (m_minimumFingers > data.fingers || m_maximumFingers < data.fingers))
        || (m_keyboardModifiers && *m_keyboardModifiers != data.keyboardModifiers)
        || (m_mouseButtons && *m_mouseButtons != data.mouseButtons)
        || (m_edges && !m_edges->contains(data.edges))) {
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

void Gesture::setThresholds(const qreal &minimum, const qreal &maximum)
{
    m_minimumThreshold = minimum;
    m_maximumThreshold = maximum;
}

void Gesture::setFingers(const uint8_t &minimum, const uint8_t &maximum)
{
    m_minimumFingers = minimum;
    m_maximumFingers = maximum;
}

void Gesture::setFingerCountIsRelevant(const bool &relevant)
{
    m_fingerCountIsRelevant = relevant;
}

void Gesture::setEdges(const std::optional<std::set<Edges>> &edges)
{
    m_edges = edges;
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
    return ((m_minimumThreshold == 0 || m_absoluteAccumulatedDelta >= m_minimumThreshold)
            && (m_maximumThreshold == 0 || m_absoluteAccumulatedDelta <= m_maximumThreshold));
}

void Gesture::setSpeed(const libgestures::GestureSpeed &speed)
{
    m_speed = speed;
}

const GestureSpeed &Gesture::speed() const
{
    return m_speed;
}

const std::optional<std::set<Edges>> &Gesture::edges() const
{
    return m_edges;
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