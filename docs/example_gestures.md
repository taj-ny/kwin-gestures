# Example gestures
All gestures provided here are instant - actions trigger immediately when the gesture begins. If you don't like that, replace ``on: begin`` with ``on: end``.

## Mouse
<details>
  <summary>Right + Draw circle - Close window</summary>

  ```yaml
  - type: stroke
    strokes: [ 'PAsAXx8PClkXEg1PDRkRRwUiFj8BLBo1ADcdKQJAIR0ISCQUEU8oCilYMQBFWDv4TlU/8FpPRORgR0fXZDtLzGMzTsReJ1O8WyFVslEZWqo4DmQA' ]
    mouse_buttons: [ right ]

    actions:
      - plasma_shortcut: kwin,Window Close
  ```
</details>
<details>
  <summary>Meta + Wheel - Volume control</summary>
  
  ```yaml
  - type: wheel
    direction: up
    keyboard_modifiers: [ meta ]

    actions:
      - input:
        - keyboard: [ volumeup ]

  - type: wheel
    direction: down
    keyboard_modifiers: [ meta ]

    actions:
      - input:
        - keyboard: [ volumedown ]
  ```
</details>
<details>
  <summary>Meta + Left/Right - Go back/forward</summary>

  ```yaml
  - type: press
    keyboard_modifiers: [ meta ]
    mouse_buttons: [ left ]
    instant: true

    actions:
      - on: begin
        input:
          - mouse: [ back ]

  - type: press
    keyboard_modifiers: [ meta ]
    mouse_buttons: [ right ]
    instant: true

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
    begin_positions: [ 0%;0% - 0.01%;0.01% ]
    press_instant: true

    actions:
      - on: begin
        command: dolphin
  ```
</details>
<details>
  <summary>Middle click top edge - Maximize window</summary>

  ```yaml
  - type: press
    mouse_buttons: [ middle ]
    begin_positions: [ 0%;0% - 100%;0.01% ]
    instant: true

    actions:
      - on: begin
        plasma_shortcut: kwin,Window Maximize
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
<details>
  <summary>Swipe 3 left/right - Go back/forward</summary>
  
  ```yaml
  - type: swipe
    fingers: 3
    direction: left

    actions:
      - on: begin
        input:
          - mouse: [ back ]

  - type: swipe
    fingers: 3
    direction: right

    actions:
      - on: begin
        input:
          - mouse: [ forward ]
  ```
  
</details>
<details>
  <summary>Rotate 2 - Volume control</summary>

  ```yaml
  - type: rotate
    fingers: 2
    direction: any

    actions:
      - on: update
        interval: -10
        input:
          - keyboard: [ volumedown ]

      - on: update
        interval: 10
        input:
          - keyboard: [ volumeup ]
  ```
</details>

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