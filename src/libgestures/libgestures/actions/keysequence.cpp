#include "keysequence.h"

namespace libgestures
{

KeySequenceGestureAction::KeySequenceGestureAction(std::shared_ptr<Input> input)
    : m_input(input)
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
            m_input->keyboardPress(s_keyMap.at(key));
        else if (action == "release")
            m_input->keyboardRelease(s_keyMap.at(key));
    }
}

void KeySequenceGestureAction::setSequence(const QString &sequence)
{
    m_sequence = sequence;
}

} // namespace libgestures