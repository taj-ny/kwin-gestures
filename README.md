# KWin Gestures
KWin Gestures allows you to create custom touchpad gestures and override existing ones.

Tested on 6.1.5, may not compile on 6.2.

# Installation
See [kwin-better-blur](https://github.com/taj-ny/kwin-effects-forceblur). Instructions will be added later.

# Configuration
There is currently no configuration UI. You need to manually modify the ``~/.config/kwingesturesrc`` file. The format may change at any time until 1.0.0 is released.

## Keys
### Type
Type of the gesture. 

``Swipe`` or ``Pinch``

### Direction
``Left``, ``Right``, ``Up``, ``Down`` if ``Type`` is ``Swipe``,

``Contracting`` or ``Expanding`` if ``Type`` is ``Pinch``.

### FingerCount (int)
Amount of fingers required to trigger the gesture. Must be at least ``3``.

Sets ``MinimumFingerCount`` and ``MaximumFingerCount``.

### MinimumFingerCount (int)
Minimum amount of fingers required to trigger the gesture. Must be at least ``3``.

### MaximumFingerCount (int)
Maximum amount of fingers required to trigger the gesture. Must be at least ``3``.

### TriggerAfterReachingThreshold (bool)
Whether to trigger the gesture immediately after the threshold is reached.

### PinchThreshold (float)
Threshold for pinch gestures.

Should be > 1 for expanding gestures and < 1 for contracting gestures.

### SwipeThresholdX (int)
Threshold for left/right swipe gestures in pixels. 

### SwipeThresholdY (int)
Threshold for up/down swipe gestures in pixels.

### Action
What to do after the gesture is triggered.

``GlobalShortcut`` - Invoke a global shortcut. Shortcuts can be found in ``~/.config/kglobalshortcutsrc``.

### Example configuration
```
[Gestures][Yakuake]
Type=Swipe
Direction=Down
FingerCount=3
TriggerAfterReachingThreshold=true
ThresholdY=50
Action=GlobalShortcut
ActionGlobalShortcutComponent=yakuake
ActionGlobalShortcutShortcut=toggle-window-state

[Gestures][CloseWindow]
Type=Pinch
Direction=Contracting
PinchThreshold=0.75
FingerCount=3
Action=GlobalShortcut
ActionGlobalShortcutComponent=kwin
ActionGlobalShortcutShortcut=Window Close
```