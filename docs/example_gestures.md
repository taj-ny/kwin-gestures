# Example gestures

### Firefox navigation
Not guaranteed to work on all keyboard layouts. It may be necessary to change the key sequence.
- Swipe 3 fingers left - Go back
- Swipe 3 fingers right - Go forward
- Swipe 3 fingers down - Refresh
```
[Gestures][Touchpad][0]
Type=Swipe
Fingers=3
TriggerWhenThresholdReached=true

[Gestures][Touchpad][0][Swipe]
Direction=Left
Threshold=10

[Gestures][Touchpad][0][Actions][0]
Type=KeySequence

[Gestures][Touchpad][0][Actions][0][Conditions][0]
WindowClassRegex=firefox

[Gestures][Touchpad][0][Actions][0][KeySequence]
Sequence=press LEFTCTRL,press LEFTBRACE,release LEFTCTRL,release LEFTBRACE


[Gestures][Touchpad][1]
Type=Swipe
Fingers=3
TriggerWhenThresholdReached=true

[Gestures][Touchpad][1][Swipe]
Direction=Right
Threshold=10

[Gestures][Touchpad][1][Actions][0]
Type=KeySequence

[Gestures][Touchpad][1][Actions][0][Conditions][0]
WindowClassRegex=firefox

[Gestures][Touchpad][1][Actions][0][KeySequence]
Sequence=press LEFTCTRL,keyboardPressKey RIGHTBRACE,release LEFTCTRL,keyboardReleaseKey RIGHTBRACE


[Gestures][Touchpad][2]
Type=Swipe
Fingers=3
TriggerWhenThresholdReached=true

[Gestures][Touchpad][2][Swipe]
Direction=Down
Threshold=10

[Gestures][Touchpad][2][Actions][0]
Type=KeySequence

[Gestures][Touchpad][2][Actions][0][Conditions][0]
WindowClassRegex=firefox

[Gestures][Touchpad][2][Actions][0][KeySequence]
Sequence=press LEFTCTRL,keyboardPressKey F5,release LEFTCTRL,keyboardReleaseKey F5
```

### Volume control
This gesture will work even if you change the direction without lifting fingers. It's recommended not to test this with audio playing just in case the interval is too small.

- Swipe 4 fingers left to decrease volume by 5%
- Swipe 4 fingers right to increase volume by 5%

```
[Gestures][Touchpad][3]
Type=Swipe
Fingers=4

[Gestures][Touchpad][3][Swipe]
Direction=LeftRight

[Gestures][Touchpad][3][Actions][0]
Type=Command
RepeatInterval=-50

[Gestures][Touchpad][3][Actions][0][Command]
Command=pactl set-sink-volume @DEFAULT_SINK@ -5%

[Gestures][Touchpad][3][Actions][1]
Type=Command
RepeatInterval=50

[Gestures][Touchpad][3][Actions][1][Command]
Command=pactl set-sink-volume @DEFAULT_SINK@ +5%
```

# Window management
- Swipe 4 fingers up to maximize window if not maximized
- Swipe 4 fingers down to unmaximize window if maximized
- Swipe 4 fingers down to minimize window if not maximized and not fullscreen
- Pinch 2 fingers in to close window

```
[Gestures][Touchpad][4]
Type=Swipe
Fingers=4
TriggerWhenThresholdReached=false

[Gestures][Touchpad][4][Swipe]
Direction=Down

[Gestures][Touchpad][4][Actions][0] # Minimize window if not fullscreen and not maximized
Type=GlobalShortcut

[Gestures][Touchpad][4][Actions][0][Conditions][0]
Negate=true
WindowState=Fullscreen|Maximized

[Gestures][Touchpad][4][Actions][0][GlobalShortcut]
Component=kwin
Shortcut=Window Minimize

[Gestures][Touchpad][4][Actions][1] # Unmaximize window if maximized
Type=GlobalShortcut

[Gestures][Touchpad][4][Actions][1][Conditions][0]
WindowState=Maximized

[Gestures][Touchpad][4][Actions][1][GlobalShortcut]
Component=kwin
Shortcut=Window Maximize


[Gestures][Touchpad][5]
Type=Swipe
Fingers=4
TriggerWhenThresholdReached=true

[Gestures][Touchpad][5][Swipe]
Direction=Up

[Gestures][Touchpad][5][Actions][0] # Maximize window if not maximized
Type=GlobalShortcut

[Gestures][Touchpad][5][Actions][0][Conditions][0]
Negate=true
WindowState=Maximized

[Gestures][Touchpad][5][Actions][0][GlobalShortcut]
Component=kwin
Shortcut=Window Maximize

[Gestures][Touchpad][6]
Type=Pinch
Fingers=2
TriggerWhenThresholdReached=true

[Gestures][Touchpad][6][Pinch]
Direction=Contracting

[Gestures][Touchpad][6][Actions][0] # Close window
Type=GlobalShortcut

[Gestures][Touchpad][6][Actions][0][GlobalShortcut]
Component=kwin
Shortcut=Window Close
```