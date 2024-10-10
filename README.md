# KWin Gestures
KWin Gestures allows you to create custom touchpad gestures and override existing ones.

# Installation
See [kwin-better-blur](https://github.com/taj-ny/kwin-effects-forceblur). Instructions will be added later.

# Configuration
There is currently no configuration UI. You need to manually modify the ``~/.config/kwingesturesrc`` file.

### Example configuration
```
[Gestures][minimize]
Type=Swipe
Direction=Down
MinimumFingerCount=3
MaximumFingerCount=3
Action=GlobalShortcut
ActionGlobalShortcutComponent=kwin
ActionGlobalShortcutShortcut=Window Minimize
```