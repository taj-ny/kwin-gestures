# Configuration
There is currently no configuration UI and I have no intention of making one.

The configuration file is located at ``~/.config/kwingestures.yml``. It is created automatically when the effect is loaded. The effect is reconfigured when the file changes.

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

| Property     | Description                                                      |
|--------------|------------------------------------------------------------------|
| **gestures** | List of gestures for this device. See *Gesture* below.           |
| speed        | Settings for how gesture speed is determined. See *Speed* below. |

## Speed
The defaults may not work for everyone, as they depend on the device's sensitivity and size.

| Property            | Description                                                                                                                                                                                                                                                                                                                                                               | Default |
|---------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|---------|
| events              | How many input events to sample in order to determine the speed at which the gesture is performed. The average of each event's delta is compared against the thresholds below. If the threshold is reached, the gesture is considered to have been performed fast, otherwise slow.<br><br>**Note**: No gestures will be triggered until all events have been sampled.     | *3*     |
| swipe_threshold     |                                                                                                                                                                                                                                                                                                                                                                           | *20*    |
| pinch_in_threshold  |                                                                                                                                                                                                                                                                                                                                                                           | *0.04*  |
| pinch_out_threshold |                                                                                                                                                                                                                                                                                                                                                                           | *0.08*  |

## Gesture

| Property      | Description                                                                                                                                                                                                                                                                                                     | Default |
|---------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|---------|
| **type**      | *hold*, *pinch*, *swipe*<br><br>For *pinch in* gestures, the scale ranges from 1.0 to 0.0. For *pinch out*, the scale is larger than 1.0.                                                                                                                                                                       | *none*  |
| **direction** | For pinch gestures: *in*, *out*, *any*<br>For swipe gestures: *left*, *right*, *up*, *down*, *left_right* (*left* and *right*), *up_down* (*up* and *down*)                                                                                                                                                     | *none*  |
| **fingers**   | The exact amount or range of fingers required to trigger this gesture.<br><br>Minimum value: *1* for *hold* gestures, *2* for *pinch* gestures, *3* for *swipe*.<br>Maximum value: Depends on how many fingers the device can detect.<br><br>Examples: ``1``, ``2-3``                                           | *none*  |
| speed         | *any*, *fast*, *slow*<br><br>The speed at which the gesture must be performed.<br><br>If a value other than *any* is specified, this gesture will cause all other gestures of the same device and type to be delayed by *Speed->events* input events until the speed is determined.                             | *any*   |
| threshold     | How far this gesture needs to progress in order to be activated.<br><br>A single number sets the minimum threshold, two numbers separated by ``-`` set both the minimum and the maximum.<br><br>A gesture with *begin* or *update* actions can't have a maximum threshold.<br><br>Examples: ``10``, ``100-200`` | *none*  |
| conditions    | List of conditions. See *Condition* below.<br><br>At least one condition (or 0 if none specified) must be satisfied in order for this gesture to be triggered.                                                                                                                                                  | *none*  |                                                                                                        
| actions       | List of actions. See *Action* below.                                                                                                                                                                                                                                                                            | *none*  |

## Condition
All specified subconditions must be satisfied in order for the condition to be satisfied.  OR conditions can be created by adding multiple conditions.

| Property     | Description                                                                                                                                                                | Default |
|--------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------|---------|
| negate       | *window_class*, *window_state*<br><br>Which properties to negate.                                                                                                          | *none*  |
| window_class | A regular expression executed on the currently focused window's resource class and resource name. If a match is not found for either, the condition will not be satisfied. | *none*  |
| window_state | *fullscreen*, *maximized*<br><br>Multiple values can be specified.<br><br>Examples: ``fullscreen``, ``fullscreen maximized``                                               | *none*  |

## Action
| Property        | Description                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         | Default |
|-----------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|---------|
| on              | *begin* - The gesture has started.<br>*end* - The gesture has ended.<br>*cancel* - The gesture has been cancelled, for example due a to a finger being lifted or the direction being changed.<br>*update* - An input event has been sent by the device. Can only be executed once, unless *interval* is set.<br>*end_cancel* - *end* or *cancel*<br><br>When the action should be triggered.<br>To make a gesture trigger when the fingers are lifted, use *ended*.<br>To make a gesture trigger immediately the specified threshold is reached, use *updated*.                                                                                                                                                                                                                                                                                                                                     | *end*   |
| interval        | Whether and how often an *update* action should be repeated.<br><br>Can be negative for all gestures except *hold*.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 | *none*  |
| block_other     | *false*, *true*<br><br>Whether this action should block other actions when executed.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                | *none*  |
| command         | Execute a command.<br><br>Example: ``dolphin``<br><br>Mutually exclusive with *keyboard* and *plasma_shortcut*.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     | *none*  |
| keyboard        | List of keyboard actions separated by space. There are two types of actions:<br>- Manual - A plus (press) or a minus (release) followed by the key, for example ``+LEFTCTRL +N -N -LEFTCTRL``. Keys must be released manually.<br>- Automatic - One or more keys separated by a ``+``, for example ``LEFTCTRL+N``. Keys are pressed in the order as they appear and then released in reverse order.<br>Both types of actions can be combined, for example ``+LEFTSHIFT T+E -LEFTSHIFT S+T`` results in ``TEst``.<br><br>Full key list: [src/libgestures/libgestures/gestures/keysequence.h](src/libgestures/libgestures/gestures/keysequence.h)<br><br>**Note:** If you misspell or forget to release a modifier key, you won't be able to release it using your keyboard and modifying the configuration file may not be possible.<br><br>Mutually exclusive with *command* and *plasma_shortcut*. | *none*  |
| plasma_shortcut | Invoke a KDE Plasma global shortcut. Format: ``[component],[shortcut]``.<br><br>Run ``qdbus org.kde.kglobalaccel \| grep /component`` for the list of components. Don't put the */component/* prefix in this file.<br>Run ``qdbus org.kde.kglobalaccel /component/$component org.kde.kglobalaccel.Component.shortcutNames`` for the list of shortcuts.<br><br>Examples: ``kwin,Window Minimize``<br><br>Mutually exclusive with *command* and *keyboard*.                                                                                                                                                                                                                                                                                                                                                                                                                                           | *none*  |
| conditions      | List of conditions. See *Condition* below.<br><br>At least one condition (or 0 if none specified) must be satisfied in order for this action to be triggered.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       | *none*  |
| threshold       | Same as *Gesture->threshold*, but only applied to this action.<br><br>*Begin* actions can't have thresholds.

# Example
```yaml
touchpad:
  speed:
    swipe_threshold: 15

  gestures:
    - type: pinch
      fingers: 2
      direction: in

      actions:
        - plasma_shortcut: kwin,Window Close
```