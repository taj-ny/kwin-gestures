# Configuration
> [!IMPORTANT]
> Until v1.0.0 is released, breaking changes may be introduced at any time.

There is currently no configuration UI. The configuration file is located at ``~/.config/kwingestures.yml``. It is created automatically when the plugin is loaded. The plugin should be reconfigured when the file changes. If it doesn't, disable and enable it manually or run ``qdbus org.kde.KWin /plugins org.kde.kwin.plugins.reconfigureplugin kwin_gestures``.

When the configuration fails to load, the error will be logged. To see it, run ``journalctl --boot=0 -g "kwin_gestures:" -n 5``. The message should contain the approximate position of where the error has occurred.
```
kwin_gestures: Failed to load configuration: Invalid swipe direction (line 4, column 17)
```

# Configuration file structure
Bolded properties must be set.

## Types
| Type                       | Description                                                                                                        |
|----------------------------|--------------------------------------------------------------------------------------------------------------------|
| bool                       | *true* or *false*                                                                                                  |
| int                        | A positive or negative number.                                                                                     |
| uint                       | A positive number.                                                                                                 |
| float                      | A positive or negative number with decimal places.                                                                 |
| list(type)                 | List containing elements of *type*.<br><br>Example: ``list(int)`` - ``[ 1, 2, 3 ]`` or:<br>- 1<br>- 2<br>- 3       |
| range(type)                | A range of numbers of *type*. Format: ``min-max``.<br><br>Example: ``range(int)`` - ``1-2``                        |
| enum(value1, value2, ...)  | A single value from the list of valid values in brackets.                                                          |
| flags(value1, value2, ...) | A list of one or multiple values from the list of valid values in brackets.<br><br>Example: ``[ value1, value2 ]`` | 

## Root
| Property    | Type       | Description                                                                | Default |
|-------------|------------|----------------------------------------------------------------------------|---------|
| autoreload  | ``bool``   | Whether the configuration should be automatically reloaded on file change. | *true*  |
| touchpad    | ``Device`` | See *Device* below.                                                        |         |

## Device
The only device supported at the time is *touchpad*.

The device is the root element in the configuration file:
```yaml
touchpad:
  gestures:
    # ...
```

| Property     | Type              | Description                                                      |
|--------------|-------------------|------------------------------------------------------------------|
| **gestures** | ``list(Gesture)`` | Gestures for this device. See *Gesture* below.                   |
| speed        | ``Speed``         | Settings for how gesture speed is determined. See *Speed* below. |

## Speed
The defaults may not work for everyone, as they depend on the device's sensitivity and size.

| Property            | Type      | Description                                                                                                                                                                                                                                                                                                                                                           | Default |
|---------------------|-----------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|---------|
| events              | ``uint``  | How many input events to sample in order to determine the speed at which the gesture is performed. The average of each event's delta is compared against the thresholds below. If the threshold is reached, the gesture is considered to have been performed fast, otherwise slow.<br><br>**Note**: No gestures will be triggered until all events have been sampled. | *3*     |
| swipe_threshold     | ``float`` |                                                                                                                                                                                                                                                                                                                                                                       | *20*    |
| pinch_in_threshold  | ``float`` |                                                                                                                                                                                                                                                                                                                                                                       | *0.04*  |
| pinch_out_threshold | ``float`` |                                                                                                                                                                                                                                                                                                                                                                       | *0.08*  |

## Gesture
See [example_gestures.md](example_gestures.md) for examples.

| Property      | Type                                                                                                                   | Description                                                                                                                                                                                                                                           | Default  |
|---------------|------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|----------|
| **type**      | ``enum(hold, pinch, swipe)``                                                                                           | For *pinch in* gestures, the scale ranges from 1.0 to 0.0. For *pinch out*, the scale is larger than 1.0.                                                                                                                                             | *none*   |
| **direction** | For pinch gestures: ``enum(in, out, any)``<br>For swipe gestures: ``enum(left, right, up, down, left_right, up_down)`` | *any*, *left_right* and *up_down* are bi-directional gestures. The direction can be changed during the gesture.                                                                                                                                       | *none*   |
| **fingers**   | ``uint`` (exact amount) or ``range(uint)``                                                                             | The exact amount or range of fingers required to trigger this gesture.<br><br>Minimum value: *1* for *hold* gestures, *2* for *pinch* gestures, *3* for *swipe*.<br>Maximum value: Depends on how many fingers the device can detect.                 | *none*   |
| speed         | ``enum(any, fast, slow)``                                                                                              | The speed at which the gesture must be performed.<br><br>If a value other than *any* is specified, this gesture will cause all other gestures of the same device and type to be delayed by *Speed.events* input events until the speed is determined. | *any*    |
| threshold     | ``float`` (min) or ``range(float)`` (min and max)                                                                      | How far this gesture needs to progress in order to be activated.<br><br>A gesture with *begin* or *update* actions can't have a maximum threshold.                                                                                                    | *none*   |
| conditions    | ``list(Condition)``                                                                                                    | See *Condition* below.<br><br>At least one condition (or 0 if none specified) must be satisfied in order for this gesture to be triggered.                                                                                                            | *none*   |
| actions       | ``list(Action)``                                                                                                       | See *Action* below.                                                                                                                                                                                                                                   | *none*   |

## Condition
All specified subconditions must be satisfied in order for the condition to be satisfied.  OR conditions can be created by adding multiple conditions.

| Property     | Type                                  | Description                                                                                                                                           | Default |
|--------------|---------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------|---------|
| negate       | ``flags(window_class, window_state)`` | Which properties to negate.                                                                                                                           | *none*  |
| window_class | ``regex``                             | Executed on the currently focused window's resource class and resource name. If a match is not found for either, the condition will not be satisfied. | *none*  |
| window_state | ``flags(fullscreen, maximized)``      |                                                                                                                                                       | *none*  | 

## Action
| Property        | Type                                              | Description                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             | Default |
|-----------------|---------------------------------------------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|---------|
| on              | ``enum(begin, end, cancel, update, end_cancel)``  | *begin* - The gesture has started.<br>*end* - The gesture has ended.<br>*cancel* - The gesture has been cancelled, for example due a to a finger being lifted or the direction being changed.<br>*update* - An input event has been sent by the device. Can only be executed once, unless *interval* is set.<br>*end_cancel* - *end* or *cancel*<br><br>When the action should be triggered.<br>To make a gesture trigger when the fingers are lifted, use *end*.<br>To make a gesture trigger immediately, use *begin*.                                                                                                                                                                                                                                                                                                                                                                | *end*   |
| interval        | ``float``                                         | Whether and how often an *update* action should be repeated. Can be negative for bi-directional gestures.<br><br>0 - no repeating                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       | *0*     |
| block_other     | ``bool``                                          | Whether this action should block other actions when executed.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           | *false* |
| keyboard        | ``list(string)``                                  | List of keyboard actions. There are two types of actions:<br>- Manual - A plus (press) or a minus (release) followed by the key, for example ``[ +leftctrl, +n, -n, -leftctrl ]``. Keys must be released manually.<br>- Automatic - One or more keys separated by a ``+``, for example ``[ leftctrl+n ]``. Keys are pressed in the order as they appear and then released in reverse order.<br>Both types of actions can be combined, for example ``[ +leftshift, t+e, -leftshift, s+t ]`` results in ``TEst``.<br><br>Full key list: [src/libgestures/libgestures/actions/keysequence.h](src/libgestures/libgestures/actions/keysequence.h)<br><br>**Note:** If you misspell or forget to release a key, you won't be able to release it using your keyboard and modifying the configuration file may not be possible.<br><br>Mutually exclusive with *command* and *plasma_shortcut*. | *none*  |
| plasma_shortcut | ``string``                                        | Invoke a KDE Plasma global shortcut. Format: ``component,shortcut``.<br><br>Run ``qdbus org.kde.kglobalaccel \| grep /component`` for the list of components. Don't put the */component/* prefix in this file.<br>Run ``qdbus org.kde.kglobalaccel /component/$component org.kde.kglobalaccel.Component.shortcutNames`` for the list of shortcuts.<br><br>Example: ``kwin,Window Minimize``<br><br>Mutually exclusive with *command* and *keyboard*.                                                                                                                                                                                                                                                                                                                                                                                                                                    | *none*  |
| conditions      | ``list(Condition)``                               | List of conditions. See *Condition* below.<br><br>At least one condition (or 0 if none specified) must be satisfied in order for this action to be triggered.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           | *none*  |
| threshold       | ``float`` (min) or ``range(float)`` (min and max) | Same as *Gesture.threshold*, but only applied to this action.<br><br>*Begin* actions can't have thresholds.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             | *none*  |

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