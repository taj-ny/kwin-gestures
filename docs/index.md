# Gestures
## Lifecycle
- activate - If a gesture's initial conditions (finger count, keyboard modifiers, mouse buttons, screen edges) are satisfied, it is added to the list of active gestures and can receive update events. If one of the activated gestures has set speed, all of them will only receive events after it is determined.
- begin - A gesture usually activates on the first update event, but that can be delayed or prevented entirely by thresholds.
- update - Before the gesture receives an update event, its update conditions (direction, speed) are checked first. If not satisfied, the gesture is cancelled.
- end
- cancel - The gesture's update conditions were not satisfied, the finger count has changed, or there is a gesture conflict (see section below). Cancel actions are only executed if the gesture has began.

## Conflicts
In order to avoid conflicts, only one gesture may be active at a time. Internally it works in a different way, but that's not important. All other gestures are cancelled immediately when:
- a gesture is updated, and has any action that has been executed, or has an update action that can be executed (satisfies conditions and thresholds) but hasn't been yet (the interval hadn't been reached)
- a gesture ends, and has an end action that can be executed
- a stroke gesture is 

Gestures are updated in order as specified in the configuration file.

In the following example, it is not necessary to set ``speed: slow`` on *Gesture 2* anymore, because when the action of *Gesture 1* is executed, *Gesture 2* will be cancelled immediately.
```yaml
# Gesture 1
- type: swipe
  direction: right
  fingers: 3
  speed: fast
  
  actions:
    - on: begin
      # ...

# Gesture 2
- type: swipe
  direction: right
  fingers: 3

  actions:
    - on: begin
      # ...
```

## Press gestures
Press gestures update every 5 ms with a delta of 5.

## Swipe gestures
Swipe gestures are limited to 4 directions. The direction is determined during the first few input events, which allows for executing actions at all points of the gesture's lifecycle.

## Stroke gestures
Stroke gestures allow you to draw any shape. The performed stroke is compared against all gestures and the one with the highest match (must be ≥0.7) is ended, while all others are cancelled. 

Strokes can be recorded using the stroke recorder at *System Settings* -> *Desktop Effects* -> *Gestures (configure)* or DBus: ``qdbus dev.taj-ny.kwin-gestures /KWinGestures recordStroke``.

Only *end* actions are supported.

# Built-in Plasma gestures
Plasma provides no way of disabling touchpad gestures. The plugin blocks them by preventing certain input events from reaching KWin's gesture handler and clients.

# Input
Input events are sent at blocked at compositor level, therefore applications that read from /dev/input may not function as expected.  

# Devices
## Mouse
### Press gestures
Mouse press gestures start immediately by default, preventing swipe gestures or normal clicks from being performed. If this behavior is not desired, *Gesture.press_instant* should be set to false.

### Wheel gestures
Wheel gestures begin, update and end instantly on every single scroll event, therefore they do not support thresholds and speed.

### Distinguishing between normal clicks and press/swipe gestures
When a mouse button is pressed, the event is blocked and the plugin gives the user 200 ms to perform any mouse movement. After that period, a press gesture is started, however if none were activated, all previously blocked buttons are pressed.

## Touchpad
### Acceleration
KWin only provides unaccelerated deltas for both scroll and swipe events.

### Edge detection
This is not possible due to KWin not providing the absolute positions of fingers. Gesture speed and min/max thresholds may be used as an alternative.

### Press (hold) gestures
Single- and two-finger press gestures begin almost immediately if the fingers have not been lifted very quickly. Three- and four-finger gestures have a significant delay added by libinput that may make those gestures annoying to use.

### Two-finger swipe gestures
Two-finger swipe gestures are achieved by treating scroll events as swipe events. As a result of this approach, the plugin does not know when fingers are lifted, so it must assume that the gesture has ended if there have been no events for 100 ms.

Another issue is that the thresholds for changing the scroll axis are quite large, which can cause problems with stroke gestures and ``direction: any`` swipe gestures utilizing the ``move_by_delta`` mouse input action.

If scrolling on touchpad edges is enabled, this feature will not work.