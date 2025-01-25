# Configuration
There is currently no configuration UI and I have no intention of making one.

The configuration file is located at ``~/.config/kwingestures.yml``. It is created automatically when the effect is loaded. Run ``qdbus org.kde.KWin /Effects org.kde.kwin.Effects.reconfigureEffect kwin_gestures`` or toggle the effect in system settings after updating it.

See [example_gestures.md](example_gestures.md) for examples.

When the configuration fails to load, the error will be logged. To see it, run ``journalctl --boot=0 -g "kwin_gestures:" -n 5``. The message should contain the approximate position of where the error has occurred.
```
kwin_gestures: Failed to load configuration: Invalid swipe direction (line 4, column 17)
```

# Configuration file structure
Bolded properties must be set.

## Device
The only device supported at the time is *touchpad*.

The device is the root element in the configuration file:
```yaml
touchpad:
  gestures:
    # ...
```

| Property     | Description                                                |
|--------------|------------------------------------------------------------|
| **gestures** | List of gestures for this device. See *Gesture* below.     |
| settings     | TODO                                                       |

## Gesture

| Property      | Description                                                                                                                                                                                                                                                                | Default |
|---------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|---------|
| **type**      | *hold*, *pinch*, *swipe*                                                                                                                                                                                                                                                   | *none*  |
| **direction** | For pinch gestures: *in*, *out*, *any*<br>For swipe gestures: *left*, *right*, *up*, *down*, *horizontal* (*left* and *right*), *vertical* (*up* and *down*)                                                                                                               | *none*  |
| **fingers**   | The exact amount or range of fingers required to trigger this gesture.<br><br>Minimum value: *1* for *hold* gestures, *2* for *pinch* gestures, *3* for *swipe*.<br>Maximum value: Depends on how many fingers the device can detect.<br><br>Examples: ``1``, ``2-3``      | *none*  |
| speed         | *any*, *fast*, *slow*<br><br>The speed at which the gesture must be performed.<br><br>If a value other than *any* is specified, this gesture will cause all other gestures of the same device and type to be delayed by *TODO* input events until the speed is determined. | *any*   |
| threshold     | TODO                                                                                                                                                                                                                                                                       | *none*  |
| conditions    | List of conditions. See *Condition* below.<br><br>At least one condition (or 0 if none specified) must be satisfied in order for this gesture to be triggered.                                                                                                             | *none*  |                                                                                                        
| actions       | List of actions. See *Action* below.                                                                                                                                                                                                                                       | *none*  |

# Condition
All specified subconditions must be satisfied in order for the condition to be satisfied.  OR conditions can be created by adding multiple conditions.

| Property     | Description                                                                                                                                                                | Default |
|--------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------|---------|
| negate       | *window_class*, *window_state*<br><br>Which properties to negate.                                                                                                          | *none*  |
| window_class | A regular expression executed on the currently focused window's resource class and resource name. If a match is not found for either, the condition will not be satisfied. | *none*  |
| window_state | *fullscreen*, *maximized*<br><br>Multiple values can be specified.<br><br>Examples: ``fullscreen``, ``fullscreen maximized``                                               | *none*  |

# Action
| Property        | Description                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       | Default |
|-----------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|---------|
| when            | *started* - The gesture has started.<br>*ended* - The gesture has ended.<br>*cancelled* - The gesture has been cancelled, for example due a to a finger being lifted or the direction being changed.<br>*updated* - An input event has been sent by the device. Can only be executed once, unless *interval* is set.<br>*ended_cancelled* - *ended* or *cancelled*<br><br>When the action should be triggered.<br>To make a gesture trigger when the fingers are lifted, use *ended*.<br>To make a gesture trigger immediately the specified threshold is reached, use *updated*.                                                                                                                                                                                                 | *ended* |
| interval        | Whether and how often an *update* action should be repeated.<br><br>Can be negative for all gestures except *hold*.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               | *none*  |
| block_other     | *false*, *true*<br><br>Whether this action should block other actions when executed.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              | *none*  |
| command         | Execute a command.<br><br>Example: ``dolphin``<br><br>Mutually exclusive with *keyboard* and *plasma_shortcut*.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   | *none*  |
| keyboard        | Send keyboard input. There are two formats, both of which are case-insensitive.<br><br>**Simple**:<br>Intended for very simple shortcuts, such as CTRL+T. Keys are pressed in the order as specified, and then released in the reverse order.<br>Examples: ``LEFTCTRL+T``, ``LEFTMETA+LEFTSHIFT+TAB``<br><br>**Complex**:<br>Intended for more complex shortcuts where more control is required. ``+KEY`` presses a key and ``-KEY`` releases it. **Keys will not be released automatically, even when the gesture ends.**<br>Example: ``+LEFTSHIFT +H -H -LEFTSHIFT +I -I`` -> Hi<br><br>Full key list: [src/libgestures/libgestures/gestures/keysequence.h](src/libgestures/libgestures/gestures/keysequence.h)<br><br>Mutually exclusive with *command* and *plasma_shortcut*. | *none*  |
| plasma_shortcut | Invoke a KDE Plasma global shortcut. Format: ``[component],[shortcut]``.<br><br>Run ``qdbus org.kde.kglobalaccel \| grep /component`` for the list of components. Don't put the */component/* prefix in this file.<br>Run ``qdbus org.kde.kglobalaccel /component/$component org.kde.kglobalaccel.Component.shortcutNames`` for the list of shortcuts.<br><br>Examples: ``kwin,Window Minimize``<br><br>Mutually exclusive with *command* and *keyboard*.                                                                                                                                                                                                                                                                                                                         | *none*  |