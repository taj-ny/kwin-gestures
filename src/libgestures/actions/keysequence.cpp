#include "keysequence.h"

KeySequenceGestureAction::KeySequenceGestureAction(qreal repeatInterval, std::shared_ptr<VirtualInputDevice> virtualInputDevice, QString sequence)
    : GestureAction(repeatInterval),
      m_virtualInputDevice(virtualInputDevice),
      m_sequence(std::move(sequence))
{
}

void KeySequenceGestureAction::execute()
{
    GestureAction::execute();
    for (const auto &command : m_sequence.split(","))
    {
        const auto action = command.split(" ")[0];
        const auto key = command.split(" ")[1];

        if (!KEY_MAP.contains(key))
            continue;

        if (action == "press")
            m_virtualInputDevice->keyboardPress(KEY_MAP.at(key));
        else if (action == "release")
            m_virtualInputDevice->keyboardRelease(KEY_MAP.at(key));
    }
}
