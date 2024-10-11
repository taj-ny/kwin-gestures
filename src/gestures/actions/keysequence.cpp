#include "keysequence.h"
#include "virtualkeyboard.h"

KeySequenceGestureAction::KeySequenceGestureAction(QString script)
    : script(std::move(script))
{
}

void KeySequenceGestureAction::execute()
{
    VirtualKeyboard virtualKeyboard;
    for (const auto &command : script.split(","))
    {
        const auto &action = command.split(" ")[0];
        const auto &key = command.split(" ")[1];

        if (!KEY_MAP.contains(key))
            continue;

        if (action == "press")
            virtualKeyboard.press(KEY_MAP.at(key));
        else if (action == "release")
            virtualKeyboard.release(KEY_MAP.at(key));
    }
}
