#include "keysequence.h"
#include "virtualkeyboard.h"
#include "xkb.h"

KeySequenceGestureAction::KeySequenceGestureAction(QString sequence)
    : m_sequence(std::move(sequence))
{
}

void KeySequenceGestureAction::execute() const
{
    VirtualKeyboard virtualKeyboard;
    for (const auto &command : m_sequence.split(","))
    {
        const auto action = command.split(" ")[0];
        const auto key = command.split(" ")[1];

        if (!KEY_MAP.contains(key))
            continue;

        if (action == "press")
            virtualKeyboard.press(KEY_MAP.at(key));
        else if (action == "release")
            virtualKeyboard.release(KEY_MAP.at(key));
    }
}
