#include "triggerhandler.h"

namespace libgestures
{

Q_LOGGING_CATEGORY(LIBGESTURES_HANDLER, "libgestures.handler", QtWarningMsg)

/**
 * Press timer interval and delta.
 */
static qreal s_pressDelta = 5;

TriggerHandler::TriggerHandler()
{
    m_pressTimer.setTimerType(Qt::PreciseTimer);
    connect(&m_pressTimer, &QTimer::timeout, this, [this] {
        pressUpdate(s_pressDelta);
    });

    registerTriggerActivator(TriggerType::Press, std::bind(&TriggerHandler::pressTriggerActivator, this));
}

void TriggerHandler::addTrigger(std::unique_ptr<Trigger> trigger)
{
    m_triggers.push_back(std::move(trigger));
}

void TriggerHandler::keyboardKey(const Qt::Key &key, const bool &state)
{
    // Lazy way of detecting modifier release during mouse gestures
    if (Input::implementation()->isSendingInput() || state) {
        return;
    }
    endTriggers(TriggerType::All);
}

bool TriggerHandler::activateTriggers(const TriggerTypes &types, const TriggerActivateEvent *event)
{
    qCDebug(LIBGESTURES_HANDLER).noquote().nospace() << "Triggers activating (types: " << types << ", fingers: " << event->fingers << ", mouseButtons: " << event->mouseButtons << ", keyboardModifiers: " << event->keyboardModifiers << ", position: " << event->position << ")";
    cancelTriggers(TriggerType::All);
    reset();

    for (const auto &[type, activator] : m_triggerActivators) {
        if (!(types & type)) {
            continue;
        }
        activator();
    }

    auto hasKeyboardModifiers = false;
    for (auto &trigger : triggers(types, event)) {
        hasKeyboardModifiers = hasKeyboardModifiers || (trigger->keyboardModifiers() && *trigger->keyboardModifiers() != Qt::KeyboardModifier::NoModifier);
        m_activeTriggers.push_back(trigger);
        qCDebug(LIBGESTURES_HANDLER).noquote() << QString("Trigger activated (name: %1)").arg(trigger->name());
    }
    if (hasKeyboardModifiers) {
        Input::implementation()->keyboardClearModifiers();
    }

    const auto triggerCount = m_activeTriggers.size();
    qCDebug(LIBGESTURES_HANDLER).noquote().nospace() << "Triggers activated (count: " << triggerCount << ", hasModifiers: " << hasKeyboardModifiers << ")";
    return triggerCount != 0;
}

bool TriggerHandler::activateTriggers(const TriggerTypes &types, const uint8_t &fingers)
{
    auto event = createActivateEvent();
    event->fingers = fingers;
    return activateTriggers(types, event.get());
}

bool TriggerHandler::activateTriggers(const TriggerTypes &types)
{
    auto event = createActivateEvent();
    return activateTriggers(types, event.get());
}

bool TriggerHandler::updateTriggers(const std::map<TriggerTypes, const TriggerUpdateEvent *> &events)
{
    TriggerTypes types{};
    for (const auto &[type, _] : events) {
        types |= type;
    }

    qCDebug(LIBGESTURES_HANDLER).noquote().nospace() << "Updating gestures (types: " << types << ")";

    auto hasTriggers = false;
    for (auto it = m_activeTriggers.begin(); it != m_activeTriggers.end();) {
        auto trigger = *it;
        const auto &type = trigger->type();
        if (!(types & type)) {
            it++;
            continue;
        }

        const auto &event = events.at(type);
        if (!trigger->canUpdate(event)) {
            trigger->cancel();
            it = m_activeTriggers.erase(it);
            continue;
        }

        hasTriggers = true;
        trigger->update(event);

        if (!m_conflictsResolved && m_activeTriggers.size() > 1) {
            qCDebug(LIBGESTURES_TRIGGER, "Cancelling conflicting triggers");
            m_conflictsResolved = true;
            if (trigger->overridesOtherTriggers()) {
                cancelTriggers(trigger);
                break;
            } else if (types & TriggerType::StrokeSwipe) { // TODO This should be in MotionTriggerHandler
                cancelTriggers(TriggerType::Swipe);
                break;
            }
        }

        it++;
    }
    return hasTriggers;
}

bool TriggerHandler::updateTriggers(const TriggerType &type, const TriggerUpdateEvent *event)
{
    return updateTriggers({ {type, event } });
}

bool TriggerHandler::endTriggers(const TriggerTypes &types, const TriggerEndEvent *event)
{
    if (!hasActiveTriggers(types)) {
        return false;
    }

    qCDebug(LIBGESTURES_HANDLER).nospace() << "Ending gestures (types: " << types << ")";

    for (const auto &[type, ender] : m_triggerEnders) {
        if (!(types & type)) {
            continue;
        }
        ender(event);
    }

    for (auto it = m_activeTriggers.begin(); it != m_activeTriggers.end();) {
        auto trigger = *it;
        if (!(types & trigger->type())) {
            it++;
            continue;
        }

        it = m_activeTriggers.erase(it);
        if (!trigger->canEnd(event)) {
            trigger->cancel();
            continue;
        }

        if (trigger->overridesOtherTriggersOnEnd()) {
            cancelTriggers(trigger);
            trigger->end();
            break;
        }

        trigger->end();
        continue;
    }
    return true;
}

bool TriggerHandler::cancelTriggers(const TriggerTypes &types)
{
    if (!hasActiveTriggers(types)) {
        return false;
    }

    qCDebug(LIBGESTURES_HANDLER).nospace() << "Cancelling triggers (types: " << types << ")";
    for (auto it = m_activeTriggers.begin(); it != m_activeTriggers.end();) {
        auto trigger = *it;
        if (!(types & trigger->type())) {
            it++;
            continue;
        }

        trigger->cancel();
        it = m_activeTriggers.erase(it);
    }
    return true;
}

void TriggerHandler::cancelTriggers(Trigger *except)
{
    qCDebug(LIBGESTURES_HANDLER).noquote().nospace() << "Cancelling triggers (except: " << except->name() << ")";
    for (auto it = m_activeTriggers.begin(); it != m_activeTriggers.end();) {
        auto gesture = *it;
        if (gesture != except) {
            gesture->cancel();
            it = m_activeTriggers.erase(it);
            continue;
        }
        it++;
    }
}

std::vector<Trigger *> TriggerHandler::triggers(const TriggerTypes &types, const libgestures::TriggerActivateEvent *event)
{
    std::vector<Trigger *> result;
    for (auto &trigger : m_triggers) {
        if (!(types & trigger->type()) || !trigger->canActivate(event)) {
            continue;
        }
        result.push_back(trigger.get());
    }
    return result;
}

std::vector<Trigger *> TriggerHandler::activeTriggers(const TriggerTypes &types)
{
    std::vector<Trigger *> result;
    for (auto &trigger : m_activeTriggers) {
        if (!(types & trigger->type())) {
            continue;
        }
        result.push_back(trigger);
    }
    return result;
}

bool TriggerHandler::hasActiveTriggers(const TriggerTypes &types)
{
    return std::any_of(m_activeTriggers.begin(), m_activeTriggers.end(), [&types](const auto &trigger) {
        return types & trigger->type();
    });
}

void TriggerHandler::pressUpdate(const qreal &delta)
{
    if (!hasActiveTriggers(TriggerType::Press)) {
        return;
    }

    qCDebug(LIBGESTURES_HANDLER).nospace() << "Event (type: Press, delta: " << delta << ")";
    TriggerUpdateEvent event;
    event.setDelta(delta);
    const auto hasGestures = updateTriggers(TriggerType::Press, &event);
    qCDebug(LIBGESTURES_HANDLER).nospace() << "Event processed (type: Pinch, hasGestures: " << hasGestures << ")";
}

std::unique_ptr<TriggerActivateEvent> TriggerHandler::createActivateEvent() const
{
    auto event = std::make_unique<TriggerActivateEvent>();
    event->keyboardModifiers = Input::implementation()->keyboardModifiers();
    return event;
}

std::unique_ptr<TriggerEndEvent> TriggerHandler::createEndEvent() const
{
    return std::make_unique<TriggerEndEvent>();
}

void TriggerHandler::reset()
{
    m_conflictsResolved = false;
}

void TriggerHandler::pressTriggerActivator()
{
    qCDebug(LIBGESTURES_HANDLER) << "Starting press timer";
    m_pressTimer.start(s_pressDelta);
}

}