#include "keysequence.h"
#include "virtualkeyboard.h"
#include "xkb.h"

KeySequenceGestureAction::KeySequenceGestureAction(qreal repeatInterval, QString sequence)
    : GestureAction(repeatInterval),
      m_sequence(std::move(sequence))
{
}

void KeySequenceGestureAction::execute()
{
    GestureAction::execute();
    VirtualInputDevice virtualKeyboard;
    for (const auto &command : m_sequence.split(","))
    {
        const auto action = command.split(" ")[0];
        const auto key = command.split(" ")[1];

        if (!KEY_MAP.contains(key))
            continue;

        if (action == "press")
            virtualKeyboard.keyboardPressKey(KEY_MAP.at(key));
        else if (action == "release")
            virtualKeyboard.keyboardReleaseKey(KEY_MAP.at(key));
    }
}
