# KWin Gestures
KWin Gestures allows you to create custom touchpad gestures and override existing ones.

Tested on 6.1.5, may not compile on 6.2.

# Installation
See [kwin-better-blur](https://github.com/taj-ny/kwin-effects-forceblur). Instructions will be added later.

# Configuration
There is currently no configuration UI. You need to manually modify the ``~/.config/kwingesturesrc`` file. The format may change at any time until 1.0.0 is released.

Run ``qdbus org.kde.KWin /Effects org.kde.kwin.Effects.reconfigureEffect kwin_gestures`` or toggle the effect in system settings after updating the file.

### Configuration file structure

- **[Gestures]**
  - **[$device]** (enum) - Which device to add this gesture for (``Touchpad``).<br>
    - **[$name]** (string) - Unique name for the gesture.
      - **Type** (enum) - ``Hold``, ``Pinch``, ``Swipe``
      - **Fingers** (int) - Number of fingers required to trigger this gesture.<br>Sets ``MinimumFingers`` and ``MaximumFingers``.<br>Minimum value is 2 for pinch gestures, 3 for hold and swipe.<br>Maximum value is 4. 
      - **MinimumFingers** (int) - Minimum number of fingers required to trigger this gesture.<br>See **Fingers** for accepted values.
      - **MaximumFingers** (int) - Maximum number of fingers required to trigger this gesture.<br>See **Fingers** for accepted values.
      - **TriggerAfterReachingThreshold** (bool) - Whether to trigger the gesture immediately after the specified threshold is reached.<br>&nbsp;
      - **[Hold]** - Configuration for hold gestures.
        - **Threshold** (int) - In milliseconds.
      - **[Pinch]** - Configuration for pinch gestures.
        - **Direction** (enum) - ``Contracting``, ``Expanding``
        - **Threshold** (float) - Should be > 1.0 for expanding gestures and < 1.0 for contracting gestures.
      - **[Swipe]** - Configuration for swipe gestures.
        - **Direction** (enum) - ``Left``, ``Right``, ``Up``, ``Down``
        - **ThresholdX** (int) - Threshold for the X axis in pixels.<br>Only used if **Direction** is ``Left`` or ``Right``.
        - **ThresholdY** (int) - Threshold for the Y axis in pixels.<br>Only used if **Direction** is ``Up`` or ``Down``.<br>&nbsp;
      - **[Actions]** - What do to when the gesture is triggered. Actions are executed in order as they appear in the configuration file.
        - **[$name]** (string) - Unique name for the action.
          - **Type** (enum)
            - ``Command`` - Run a command. See **[Command]** for configuration.
            - ``GlobalShortcut`` - Invoke a global shortcut. See **[GlobalShortcut]** for configuration.
          - **When** (enum) - TODO<br>&nbsp;
          - **[Command]** - Configuration for the GlobalShortcut action.
            - **Command** (string) - The command to run.
          - **[GlobalShortcut]** - Configuration for the GlobalShortcut action.
            - **Component** (string) - TODO
            - **Shortcut** (string) - TODO

### Example
```
[Gestures][Touchpad][Close Window]
Type=Pinch
Fingers=2
TriggerAfterReachingThreshold=true

[Gestures][Touchpad][Close Window][Pinch]
Direction=Contracting
Threshold=0.9

[Gestures][Touchpad][Close Window][Actions][0]
Type=GlobalShortcut

[Gestures][Touchpad][Close Window][Actions][0][GlobalShortcut]
Component=kwin
Shortcut=Window Close


[Gestures][Touchpad][Yakuake]
Type=Swipe
Fingers=4
TriggerAfterReachingThreshold=true

[Gestures][Touchpad][Yakuake][Swipe]
Direction=Down
ThresholdY=10

[Gestures][Touchpad][Yakuake][Actions][0]
Type=GlobalShortcut

[Gestures][Touchpad][Yakuake][Actions][0][GlobalShortcut]
Component=yakuake
Shortcut=toggle-window-state
```