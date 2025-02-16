# Example gestures
Some of the gestures provided here are built-in and only serve as examples of what is possible. It's recommended to use the built-in equivalents, as they

Actions trigger immediately when the gesture begins. If you don't like that, replace ``on: begin`` with ``on: end``.

<details>
  <summary>Firefox/Dolphin navigation</summary>

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
</details>

<details>
  <summary>Volume control</summary>

  This is an example of a gesture with repeating actions. It's possible to change the swipe direction during the gesture.
  - Swipe 4 fingers left to decrease volume by 5%
  - Swipe 4 fingers right to increase volume by 5%

  ```yaml
  - type: swipe
    fingers: 4
    direction: left_right

    actions:
      - on: update
        interval: -50
        command: pactl set-sink-volume @DEFAULT_SINK@ -5%

      - on: update
        interval: 50
        command: pactl set-sink-volume @DEFAULT_SINK@ +5%
  ```
</details>

<details>
  <summary>Window drag (built-in)</summary>

  Built-in action equivalent: *window_drag*

  Swipe 3 fingers to drag the window. Swipe gestures have a different acceleration profile. You can change *Device.delta_multiplier* to make the gesture faster or slower.
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
  <summary>Window management</summary>

  Built-in action equivalent: *window_close*, *window_maximize*, *window_minimize*, *window_restore*, *window_tile_left*, *window_tile_right*

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
</details>

<details>
  <summary>Window switching</summary>

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
</details>

# Community-made gestures
Created an interesting collection of gestures? Feel free to add them here.