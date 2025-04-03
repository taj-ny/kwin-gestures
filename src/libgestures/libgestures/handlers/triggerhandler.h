#pragma once

#include "libgestures/triggers/trigger.h"

#include <QTimer>

Q_DECLARE_LOGGING_CATEGORY(LIBGESTURES_HANDLER)

namespace libgestures
{

class TriggerHandler : public QObject
{
    Q_OBJECT

public:
    virtual ~TriggerHandler() = default;

    void addTrigger(std::unique_ptr<Trigger> trigger);

    void keyboardKey(const Qt::Key &key, const bool &state);

protected:
    TriggerHandler();

    void registerTriggerActivator(const TriggerType &type, const std::function<void()> &activator);
    void registerTriggerEnder(const TriggerType &type, const std::function<void()> &ender);

    /**
     * Cancels all active triggers and activates triggers of the specified types eligible for activation.
     * @return Whether any triggers have been activated.
     */
    bool activateTriggers(const TriggerTypes &types, const TriggerActivateEvent *event);
    /**
     * @see activateTriggers(const TriggerTypes &, const TriggerActivateEvent *)
     */
    bool activateTriggers(const TriggerTypes &types, const uint8_t &fingers);
    /**
     * @see activateTriggers(const TriggerTypes &, const TriggerActivateEvent *)
     */
    bool activateTriggers(const TriggerTypes &types);
    /**
     * Updates triggers of multiple types.
     * @return Whether there are any active triggers.
     */
    bool updateTriggers(const std::map<TriggerTypes, const TriggerUpdateEvent *> &events);
    bool updateTriggers(const TriggerType &type, const TriggerUpdateEvent *event);
    /**
     * Ends the specified types of triggers.
     * @return Whether there were any active triggers of the specified types.
     */
    bool endTriggers(const TriggerTypes &types, const TriggerEndEvent *event);
    /**
     * @see endTriggers(const TriggerTypes &, const TriggerEndEvent *)
     */
    bool endTriggers(const TriggerTypes &types);
    /**
     * Cancels the specified types of triggers.
     * @return Whether there were any active triggers of the specified types.
     */
    bool cancelTriggers(const TriggerTypes &types);
    /**
     * Cancels all triggers leaving only the specified one.
     */
    void cancelTriggers(Trigger *except);

    /**
     * @return Triggers of the specified types eligible for activation.
     */
    std::vector<Trigger *> triggers(const TriggerTypes &types, const TriggerActivateEvent *event);
    /**
     * @return Active triggers of the specified types.
     */
    std::vector<Trigger *> activeTriggers(const TriggerTypes &types);
    /**
     * @return Whether there are any triggers of the specified types.
     */
    bool hasActiveTriggers(const TriggerTypes &types);

    virtual std::unique_ptr<TriggerActivateEvent> createActivateEvent() const;
    virtual std::unique_ptr<TriggerEndEvent> createEndEvent() const;

    /**
     * Called before a trigger is activated.
     */
    virtual void triggerActivating(Trigger *trigger);
    virtual void reset();

    void pressUpdate(const qreal &delta);

private:
    void pressTriggerActivator();

    /**
     * Whether conflicting triggers have been cancelled since activation.
     */
    bool m_conflictsResolved = false;
    bool m_beganTriggers = false;

    /**
     * Updates hold triggers.
     */
    QTimer m_pressTimer;

    std::map<TriggerType, std::function<void()>> m_triggerActivators;
    std::map<TriggerType, std::function<void()>> m_triggerEnders;

    std::vector<std::unique_ptr<Trigger>> m_triggers;
    std::vector<Trigger *> m_activeTriggers;
};

}