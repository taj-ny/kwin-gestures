# Configuration
There is currently no configuration UI and there probably won't be one. The configuration file is located at ``~/.config/kwingesturesrc``. The format may change at any time until 1.0.0 is released. Any changes will be mentioned in the release description.

Run ``qdbus org.kde.KWin /Effects org.kde.kwin.Effects.reconfigureEffect kwin_gestures`` or toggle the effect in system settings after updating the file.

See [example_gestures.md](example_gestures.md) for examples.

### Configuration file structure

- **[Gestures]**
    - **[$Device]** (enum) - Which device to add this gesture for (``Touchpad``).<br>
        - **[$GestureId]** (int) - Unique ID for the gesture.
            - **Type** (enum) - ``Hold``, ``Pinch``, ``Swipe``
            - **Fingers** (int) - Number of fingers required to trigger this gesture.<br>Sets **MinimumFingers** and **MaximumFingers**.<br>Minimum value: **2** for hold and pinch gestures, **3** for swipe.<br>Default: **none**
            - **MinimumFingers** (int) - Minimum number of fingers required to trigger this gesture.<br>See **Fingers** for accepted values.<br>Default: **none**
            - **MaximumFingers** (int) - Maximum number of fingers required to trigger this gesture.<br>See **Fingers** for accepted values.<br>Default: **none**
            - **TriggerWhenThresholdReached** (bool) - Whether to trigger the gesture immediately after the specified threshold is reached.<br>Default: **false**&nbsp;
            - **[Hold]** - Configuration for hold gestures.
                - **Threshold** (int) - In milliseconds.<br>Default: **0**
            - **[Pinch]** - Configuration for pinch gestures.
                - **Direction** (enum)
                    - ``Contracting``
                    - ``Expanding``
                    - ``Any`` - Either contracting or expanding, intended for repeating actions.
                - **Threshold** (float) - Should be >= 1.0 for expanding gestures and =< 1.0 for contracting gestures.<br>Default: **1.0**
            - **[Swipe]** - Configuration for swipe gestures.
                - **Direction** (enum)
                    - ``Left``
                    - ``Right``
                    - ``Up``
                    - ``Down``
                    - ``LeftRight`` - Either left or right, intended for repeating actions.
                    - ``UpDown`` - Either up or down, intended for repeating actions.
                - **Threshold** (int) - Threshold in pixels for the X axis if **Direction** is ``Left``, ``Right`` or ``LeftRight``, and Y axis if **Direction** is ``Up``, ``Down`` or ``UpDown``.<br>Must be positive.<br>Default: **0**<br>&nbsp;
            - **[Conditions]** - At least one condition (or 0 if none specified) must be satisfied in order for this gesture to be triggered.
                - **[$ConditionId]** (int) - Unique ID for this condition.
                    - **Negate** (bool) - If true, this condition will be satisfied only when none of its specified properties are.<br>Default: **false**
                    - **WindowClassRegex** (string) - A regular expression executed on the currently focused window's resource class and resource name. If a match is not found for either, the condition will not be satisfied.<br>Default: **none**
                    - **WindowState** (enum) - ``Fullscreen``, ``Maximized``. Values can be combined using the | separator, For example, ``Fullscreen|Maximized`` will match either fullscreen or maximized windows.<br>Default: **none**<br>&nbsp;
            - **[Actions]** - What do to when the gesture is triggered. Actions are executed in order as they appear in the configuration file.
                - **[$ActionId]** (int) - Unique ID for this action.
                    - **Type** (enum)
                        - ``Command`` - Run a command.
                        - ``GlobalShortcut`` - Invoke a global shortcut.
                        - ``KeySequence`` - Send keystrokes.
                    - **RepeatInterval** (int/float) - Whether and how often this action should repeat.<br>Can be negative for all gestures except hold.<br>Default: **0 (no repeating)**
                    - **BlockOtherActions** (bool) - Whether this action should block other actions if triggered.<br>Default: **false**<br>&nbsp;
                    - **[Command]** - Configuration for the GlobalShortcut action.
                        - **Command** (string) - The command to run.<br>Default: **none**
                    - **[GlobalShortcut]** - Configuration for the GlobalShortcut action.
                        - **Component** (string) - Run ``qdbus org.kde.kglobalaccel | grep /component`` for the list of components. Don't put the ``/component/`` prefix here.<br>Default: **none**
                        - **Shortcut** (string) - Run ``qdbus org.kde.kglobalaccel /component/$component org.kde.kglobalaccel.Component.shortcutNames`` for the list of shortcuts.<br>Default: **none**
                    - **[KeySequence]** - Configuration for the KeySequence action.
                        - **Sequence** (string) - The key sequence to run. Case-sensitive. Invalid format will cause a crash (for now). For the full list of keys see [src/gestures/actions/keysequence.h](src/gestures/actions/keysequence.h).<br>Example: ``press LEFTCTRL,keyboardPressKey T,release LEFTCTRL,keyboardReleaseKey T``.<br>Default: **none**<br>&nbsp;
                    - **[Conditions]** - Same as **[Gestures][\$Device][$GestureId][Conditions]**, but only applied to this action.
