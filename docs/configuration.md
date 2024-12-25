# Configuration
> [!NOTE]
> The configuration format may change at any time until v1.0.0 is released. You will have to make manual changes to your configuration file. All changes will be mentioned in the release description.

There is currently no configuration UI and I have no intention of making one.

The configuration file is located at ``~/.config/kwingesturesrc`` and **is not created automatically after installation**. Run ``qdbus org.kde.KWin /Effects org.kde.kwin.Effects.reconfigureEffect kwin_gestures`` or toggle the effect in system settings after updating it.

See [example_gestures.md](example_gestures.md) for examples.

Feel free to open an issue if something is unclear for you.

### Configuration file structure
- **[Gestures]**
    - **[$Device]** (enum) - Which device to add this gesture for (currently only ``Touchpad`` is supported).<br>
        - **[GestureRecognizer]** - Configuration for the gesture recognizer.
            - **[Speed]** - Configuration for the determination of gesture speed. If the fast threshold is reached, the gesture will be treated as a fast gesture, otherwise as a slow one.
                - **SampledEvents** (int) - How many input events to sample in order to determine the gesture speed. The deltas of each event are summed and compared against the specified thresholds.<br>Default: **5**<br><br>
              
                - **SwipeThreshold** (int)<br>Default: **300**<br><br>
              
                - **PinchContractingThreshold** (float)<br>Default: **0.4**
                - **PinchExpandingThreshold** (float)<br>Default: **2.0**<br><br>
        - **[$GestureId]** (int) - Unique ID for the gesture. Gestures are handled in ascending order by their ID.
            - **Type** (enum, **required**) - ``Hold``, ``Pinch``, ``Swipe``<br>Default: **none**
            - **Direction** (enum, **required**)
                - For pinch gestures:
                    - ``Contracting`` - Pinch in.
                    - ``Expanding`` - Pinch out.
                    - ``Any`` - Either contracting or expanding.
                - For swipe gestures:
                    - ``Left``
                    - ``Right``
                    - ``Up``
                    - ``Down``
                    - ``LeftRight`` - Either left or right.
                    - ``UpDown`` - Either up or down.<br>
            - **Speed** (enum) - ``Slow``, ``Fast``, ``Any``<br>If a value other than ``Any`` is specified, this gesture will cause all other gestures of the same device and type to be delayed by **[Gestures][\$Device][GestureRecognizer][Speed] SampledEvents** input events until the speed is determined.<br>Default: **Any**
            - **Fingers** (int) - Number of fingers required to trigger this gesture.<br>Sets **MinimumFingers** and **MaximumFingers**.<br>Minimum value: **1** for hold gestures, **2** for pinch gestures, **3** for swipe.<br>Maximum value: Depends on how many fingers the device can detect.<br>Default: **none**
            - **MinimumFingers** (int) - Minimum number of fingers required to trigger this gesture.<br>See **Fingers**.<br>Default: **none**
            - **MaximumFingers** (int) - Maximum number of fingers required to trigger this gesture.<br>See **Fingers**.<br>Default: **none**
            - **MinimumThreshold** - TODO
            - **MaximumThreshold** - TODO<br><br>
                  
            - **[Conditions]** - At least one condition (or 0 if none specified) must be satisfied in order for this gesture to be triggered.
                - **[$ConditionId]** (int) - Unique ID for this condition. Conditions are handled in ascending order by their ID.
                    - **Negate** (bool) - If true, this condition will be satisfied only when none of its specified subconditions are.<br>Default: **false**
                    - **WindowClassRegex** (string) - A regular expression executed on the currently focused window's resource class and resource name. If a match is not found for either, the condition will not be satisfied.<br>Default: **none**
                    - **WindowState** (enum) - ``Fullscreen``, ``Maximized``. Values can be combined using the | separator, For example, ``Fullscreen|Maximized`` will match either fullscreen or maximized windows.<br>Default: **none**<br><br>
                  
            - **[Actions]** - What do to when the gesture is triggered.
                - **[$ActionId]** (int) - Unique ID for this action. Actions are handled in ascending order by their ID.
                    - **Type** (enum)
                        - ``Command`` - Run a command.
                        - ``GlobalShortcut`` - Invoke a global shortcut (KDE).
                        - ``KeySequence`` - Send keystrokes.<br>
                        **Required**
                    - **When** (enum) - When the action should be triggered.<br>
                        For actions that should trigger after the fingers have been lifted, use ``Ended`` and optionally specify **MinimumThreshold** and **MaximumThreshold**.<br>
                        For actions that should trigger immediately when the specified **MinimumThreshold** is reached, use ``Updated``. <br>
                        - ``Started`` - When the gesture has started.
                        - ``Ended`` - When the gesture has ended.
                        - ``Cancelled`` - When the gesture has been cancelled. A gesture can be cancelled for example when a finger is lifted or the direction changes.
                        - ``Updated`` - When the gesture has been updated. Can only be executed once, unless **RepeatInterval** is specified.
                        - ``EndedOrCancelled`` - Both ``Ended`` and ``Cancelled``.
                    - **RepeatInterval** (int/float) - Whether and how often an ``Update`` action should repeat.<br>Can be negative for all gestures except hold.<br>Default: **0 (no repeating)**
                    - **BlockOtherActions** (bool) - Whether this action should block other actions if triggered.<br>Default: **false**
                    - **MinimumThreshold** - Same as **[Gestures][\$Device][$GestureId] MinimumThreshold**, but only applied to this action.<br>
                    - **MinimumThreshold** - Same as **[Gestures][\$Device][$GestureId] MaximumThreshold**, but only applied to this action.<br><br>
                  
                    - **[Command]** - Configuration for the GlobalShortcut action.
                        - **Command** (string, **required**) - The command to run.<br>Default: **none**<br>**Required**
                    - **[GlobalShortcut]** - Configuration for the GlobalShortcut action.
                        - **Component** (string, **required**) - Run ``qdbus org.kde.kglobalaccel | grep /component`` for the list of components. Don't put the ``/component/`` prefix here.<br>Default: **none**
                        - **Shortcut** (string, **required**) - Run ``qdbus org.kde.kglobalaccel /component/$component org.kde.kglobalaccel.Component.shortcutNames`` for the list of shortcuts.<br>Default: **none**
                    - **[KeySequence]** - Configuration for the KeySequence action.
                        - **Sequence** (string, **required**) - The key sequence to run. Keys can be pressed in one action and released in another.<br>For the full list of keys see [src/gestures/actions/keysequence.h](src/gestures/actions/keysequence.h).<br>Example: ``press LEFTCTRL,press T,release LEFTCTRL,release T``.<br>Default: **none**<br><br>
                    - **[Conditions]** - Same as **[Gestures][\$Device][$GestureId][Conditions]**, but only applied to this action.