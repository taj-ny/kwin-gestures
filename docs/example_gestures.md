# Example gestures

### Firefox navigation
Not guaranteed to work on all keyboard layouts. It may be necessary to change the key sequence.
- Swipe 3 fingers left - Go back
- Swipe 3 fingers right - Go forward
- Swipe 3 fingers down - Refresh

```yaml
- type: swipe
  fingers: 3
  direction: left

  conditions:
    - window_class: firefox

  actions:
    - on: begin
      keyboard: LEFTCTRL+LEFTBRACE

- type: swipe
  fingers: 3
  direction: right

  conditions:
    - window_class: firefox

  actions:
    - on: begin
      keyboard: LEFTCTRL+RIGHTBRACE

- type: swipe
  fingers: 3
  direction: down

  conditions:
    - window_class: firefox

  actions:
    - on: begin
      keyboard: F5
```

### Volume control
This gesture will work even if you change the direction without lifting fingers. It's recommended not to test this with audio playing just in case the interval is too small.

- Swipe 4 fingers left to decrease volume by 5%
- Swipe 4 fingers right to increase volume by 5%

```yaml
- type: swipe
  fingers: 4
  direction: horizontal

  actions:
    - on: update
      interval: -50
      command: pactl set-sink-volume @DEFAULT_SINK@ -5%

    - on: update
      interval: 50
      command: pactl set-sink-volume @DEFAULT_SINK@ +5%
```

# Window management
- Swipe 4 fingers up to maximize window if not maximized
- Swipe 4 fingers down to unmaximize window if maximized
- Swipe 4 fingers down to minimize window if not maximized and not fullscreen
- Pinch 2 fingers in to close window

```yaml
- type: swipe
  fingers: 4
  direction: down

  actions:
    - on: begin # Unmaximize window if maximized
      plasma_shortcut: kwin,Window Maximize
      block_other: y # Prevent the minimize window action from triggering during the same gesture

      conditions:
        - window_state: maximized

    - on: begin # Minimize window if not fullscreen and not maximized
      plasma_shortcut: kwin,Window Minimize

      conditions:
        - negate: window_state
          window_state: fullscreen maximized

- type: swipe
  fingers: 4
  direction: up

  actions:
    - on: begin # Maximize window if not maximized
      plasma_shortcut: kwin,Window Maximize

      conditions:
        - negate: window_state
          window_state: maximized

- type: pinch
  fingers: 2
  direction: in

  actions:
    - on: begin
      plasma_shortcut: kwin,Window Close
```

# Window switching
```yaml
# Window switching
- type: swipe
  direction: horizontal
  fingers: 4
  speed: fast

  actions:
    - on: begin
      keyboard: +LEFTALT TAB

    - on: update
      interval: -100
      keyboard: LEFTSHIFT+TAB

    - on: update
      interval: 100
      keyboard: TAB

    - on: end_cancel
      keyboard: -LEFTALT


# Quick window switching (left)
- type: swipe
  direction: left
  fingers: 4
  speed: slow

  actions:
    - on: begin
      keyboard: LEFTALT+LEFTSHIFT+TAB

# Quick window switching (right)
- type: swipe
  direction: right
  fingers: 4
  speed: slow

  actions:
    - on: begin
      keyboard: LEFTALT+TAB
```