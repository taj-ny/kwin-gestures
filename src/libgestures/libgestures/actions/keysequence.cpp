#include "keysequence.h"

namespace libgestures
{

KeySequenceGestureAction::KeySequenceGestureAction(qreal repeatInterval, std::shared_ptr<Input> virtualInputDevice, QString sequence)
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

        if (!s_keyMap.contains(key))
            continue;

        if (action == "press")
            m_virtualInputDevice->keyboardPress(s_keyMap.at(key));
        else if (action == "release")
            m_virtualInputDevice->keyboardRelease(s_keyMap.at(key));
    }
}

} // namespace libgestures