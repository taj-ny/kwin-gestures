# Example gestures
All gestures provided here are instant - actions trigger immediately when the gesture begins. If you don't like that, replace ``on: begin`` with ``on: end``.

Some gestures may not be compatible with each other, as they use the same direction, finger amount and speed.

## Mouse
<details>
  <summary>Meta + Wheel - Volume control</summary>
  
  ```yaml
  - type: wheel
    direction: up_down
    keyboard_modifiers: [ meta ]

    actions:
      - on: update
        interval: '+'
        input:
          - keyboard: [ volumedown ]

      - on: update
        interval: '-'
        input:
          - keyboard: [ volumeup ]
  ```
</details>
<details>
  <summary>Meta + Left/Right - Go back/forward</summary>

  ```yaml
  - type: press
    keyboard_modifiers: [ meta ]
    mouse_buttons: [ left ]

    actions:
      - on: begin
        input:
          - mouse: [ back ]

  - type: press
    keyboard_modifiers: [ meta ]
    mouse_buttons: [ right ]

    actions:
      - on: begin
        input:
          - mouse: [ forward ]
  ```
</details>
<details>
  <summary>Meta + Left + Swipe up/down - Home/End</summary>
  
  ```yaml
  - type: swipe
    direction: up
    keyboard_modifiers: [ meta ]
    mouse_buttons: [ left ]

    actions:
      - on: begin
        input:
          - keyboard: [ leftctrl+home ]

  - type: swipe
    direction: down
    keyboard_modifiers: [ meta ]
    mouse_buttons: [ left ]

    actions:
      - on: begin
        input:
          - keyboard: [ leftctrl+end ]
  ```
</details>
<details>
  <summary>Left click top left corner - Open dolphin</summary>

  ```yaml
  - type: press
    mouse_buttons: [ left ]
    edges: [ top_left ]
    press_instant: true

    actions:
      - on: begin
        command: dolphin
  ```
</details>

## Touchpad
<details>
  <summary>Swipe 3 - Window drag</summary>

  Swipe gestures have a different acceleration profile. You can change *Device.delta_multiplier* to make the gesture faster or slower.

  ```yaml
  - type: swipe
    direction: any
    fingers: 3

    actions:
      - on: begin
        input:
          - keyboard: [ +leftmeta ]
          - mouse: [ +left ]

      - on: update
        input:
          - mouse: [ move_by_delta ]

      - on: end_cancel
        input:
          - keyboard: [ -leftmeta ]
          - mouse: [ -left ]
```
</details>

## Firefox/Dolphin navigation
Not guaranteed to work on all keyboard layouts. It may be necessary to change the key sequence.
- Swipe 3 fingers left - Go back
- Swipe 3 fingers right - Go forward

```yaml
# Go back
- type: swipe
  fingers: 3
  direction: left

  actions:
    # Firefox
    - on: begin
      input:
        - keyboard: [ leftctrl+leftbrace ]

      conditions:
        - window_class: firefox

    # Dolphin
    - on: begin
      input:
        - keyboard: [ backspace ]

      conditions:
        - window_class: dolphin

# Go forward
- type: swipe
  fingers: 3
  direction: right

  actions:
    # Firefox
    - on: begin
      input:
        - keyboard: [ leftctrl+rightbrace ]

      conditions:
        - window_class: firefox

    # Dolphin
    - on: begin
      input:
        - keyboard: [ leftalt+right ]

      conditions:
        - window_class: dolphin
```

## Volume control
This is an example of a gesture with repeating actions. It's possible to change the direction during the gesture.

Stop all audio before trying this, as the threshold may be too small for some devices.

- Rotate 2 fingers counterclockwise to decrease volume by 5%
- Rotate 2 fingers clockwise to increase volume by 5%

```yaml
- type: rotate
  fingers: 2
  direction: any

  actions:
    - on: update
      interval: -10
      command: pactl set-sink-volume @DEFAULT_SINK@ -5%

    - on: update
      interval: 10
      command: pactl set-sink-volume @DEFAULT_SINK@ +5%
```

## Window management
- Swipe 4 fingers up to maximize window if not maximized
- Swipe 4 fingers down to unmaximize window if maximized
- Swipe 4 fingers down to minimize window if not maximized and not fullscreen
- Swipe 4 fingers left quickly to tile the window left
- Swipe 4 fingers right quickly to tile the window right
- Pinch 2 fingers in to close window

```yaml
# Unmaximize/minimize
- type: swipe
  fingers: 4
  direction: down

  actions:
    # Unmaximize window if maximized
    - on: begin
      plasma_shortcut: kwin,Window Maximize
      block_other: true # Prevent the minimize window action from triggering during the same gesture

      conditions:
        - window_state: [ maximized ]

    # Minimize window if not fullscreen and not maximized
    - on: begin
      plasma_shortcut: kwin,Window Minimize

      conditions:
        - negate: [ window_state ]
          window_state: [ fullscreen, maximized ]

# Maximize
- type: swipe
  fingers: 4
  direction: up

  actions:
    # Maximize window if not already maximized
    - on: begin
      plasma_shortcut: kwin,Window Maximize

      conditions:
        - negate: [ window_state ]
          window_state: [ maximized ]

# Close window
- type: pinch
  fingers: 2
  direction: in

  actions:
    - on: begin
      plasma_shortcut: kwin,Window Close

# Tile left
- type: swipe
  fingers: 4
  direction: left
  speed: fast

  actions:
    - on: begin
      plasma_shortcut: kwin,Window Quick Tile Left

# Tile right
- type: swipe
  fingers: 4
  direction: right
  speed: fast

  actions:
    - on: begin
      plasma_shortcut: kwin,Window Quick Tile Right
```

## Window switching
- Swipe 4 fingers left/right slowly to switch window
- Swipe 4 fingers left/right fast to open the alt+tab menu

```yaml
# Window switching
- type: swipe
  direction: left_right
  fingers: 4
  speed: fast

  actions:
    - on: begin
      input:
        - keyboard: [ +leftalt, tab ]

    - on: update
      interval: -75
      input:
        - keyboard: [ leftshift+tab ]

    - on: update
      interval: 75
      input:
        - keyboard: [ tab ]

    - on: end_cancel
      input:
        - keyboard: [ -leftalt ]

# Quick window switching (left)
- type: swipe
  direction: left
  fingers: 4
  speed: slow

  actions:
    - on: begin
      input:
        - keyboard: [ leftalt+leftshift+tab ]

# Quick window switching (right)
- type: swipe
  direction: right
  fingers: 4
  speed: slow

  actions:
    - on: begin
      input:
        - keyboard: [ leftalt+tab ]
```

## KRunner
- Press 4 fingers to launch KRunner

```yaml
# Launch KRunner
- type: hold
  fingers: 4

  actions:
    - on: begin
      plasma_shortcut: org_kde_krunner_desktop,_launch
```
