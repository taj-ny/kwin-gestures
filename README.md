# KWin Gestures
Custom touchpad gestures for KDE Plasma 6.

Tested on 6.1.5 and 6.2. X11 is not supported.

# Features
- Override built-in gestures
- Application-specific gestures

# Installation
<details>
  <summary>NixOS (flakes)</summary>
  <br>

``flake.nix``:
  ```nix
  {
    inputs = {
      nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";

      kwin-gestures = {
        url = "github:taj-ny/kwin-gestures";
        inputs.nixpkgs.follows = "nixpkgs";
      };
    };
  }
  ```

  ```nix
  { inputs, pkgs, ... }:

  {
    environment.systemPackages = [
      inputs.kwin-gestures.packages.${pkgs.system}.default
    ];
  }
  ```
</details>

# Building from source
### Dependencies
> [!NOTE]
> The dependencies were copied from another project. Not all of them are necessary.

- CMake
- Extra CMake Modules
- Plasma 6
- Qt 6
- KF6
- KWin development packages

<details>
  <summary>Arch Linux</summary>
  <br>

  ```
  sudo pacman -S base-devel git extra-cmake-modules qt6-tools
  ```
</details>

<details>
  <summary>Debian-based (KDE Neon, Kubuntu, Ubuntu)</summary>
  <br>

  ```
  sudo apt install git cmake g++ extra-cmake-modules qt6-tools-dev kwin-dev libkf6configwidgets-dev gettext libkf6crash-dev libkf6globalaccel-dev libkf6kio-dev libkf6service-dev libkf6notifications-dev libkf6kcmutils-dev libkdecorations2-dev
  ```
</details>

<details>
  <summary>Fedora</summary>
  <br>

  ```
  sudo dnf install git cmake extra-cmake-modules gcc-g++ kf6-kwindowsystem-devel plasma-workspace-devel libplasma-devel qt6-qtbase-private-devel qt6-qtbase-devel cmake kwin-devel extra-cmake-modules kwin-devel kf6-knotifications-devel kf6-kio-devel kf6-kcrash-devel kf6-ki18n-devel kf6-kguiaddons-devel libepoxy-devel kf6-kglobalaccel-devel kf6-kcmutils-devel kf6-kconfigwidgets-devel kf6-kdeclarative-devel kdecoration-devel kf6-kglobalaccel kf6-kdeclarative libplasma kf6-kio qt6-qtbase kf6-kguiaddons kf6-ki18n wayland-devel
  ```
</details>

<details>
  <summary>openSUSE</summary>
  <br>

  ```
  sudo zypper in git cmake-full gcc-c++ kf6-extra-cmake-modules kcoreaddons-devel kguiaddons-devel kconfigwidgets-devel kwindowsystem-devel ki18n-devel kiconthemes-devel kpackage-devel frameworkintegration-devel kcmutils-devel kirigami2-devel "cmake(KF6Config)" "cmake(KF6CoreAddons)" "cmake(KF6FrameworkIntegration)" "cmake(KF6GuiAddons)" "cmake(KF6I18n)" "cmake(KF6KCMUtils)" "cmake(KF6KirigamiPlatform)" "cmake(KF6WindowSystem)" "cmake(Qt6Core)" "cmake(Qt6DBus)" "cmake(Qt6Quick)" "cmake(Qt6Svg)" "cmake(Qt6Widgets)" "cmake(Qt6Xml)" "cmake(Qt6UiTools)" "cmake(KF6Crash)" "cmake(KF6GlobalAccel)" "cmake(KF6KIO)" "cmake(KF6Service)" "cmake(KF6Notifications)" libepoxy-devel kwin6-devel
  ```
</details>

### Building
```sh
git clone https://github.com/taj-ny/kwin-gestures
cd kwin-gestures
mkdir build
cd build
cmake ../ -DCMAKE_INSTALL_PREFIX=/usr
make
sudo make install
```

Remove the *build* directory when rebuilding the effect.

# Usage
> [!NOTE]
> If the effect stops working after a system upgrade, you will need to rebuild it.

1. Install the plugin.
2. Open the Desktop Effects page in System Settings.
3. Enable the *Gestures* effect in the *Tools* category.

# Configuration
There is currently no configuration UI and it may be a while before it's added. For now, you need to manually modify the ``~/.config/kwingesturesrc`` file. The format may change at any time until 1.0.0 is released.

Run ``qdbus org.kde.KWin /Effects org.kde.kwin.Effects.reconfigureEffect kwin_gestures`` or toggle the effect in system settings after updating the file.

### Configuration file structure

- **[Gestures]**
  - **[$Device]** (enum) - Which device to add this gesture for (``Touchpad``).<br>
    - **[$GestureId]** (int) - Unique ID for the gesture.
      - **Type** (enum) - ``Hold``, ``Pinch``, ``Swipe``
      - **Fingers** (int) - Number of fingers required to trigger this gesture.<br>Sets **MinimumFingers** and **MaximumFingers**.<br>Minimum value: **2** for hold and pinch gestures, **3** for swipe.<br>Default: **none** 
      - **MinimumFingers** (int) - Minimum number of fingers required to trigger this gesture.<br>See **Fingers** for accepted values.<br>Default: **none**
      - **MaximumFingers** (int) - Maximum number of fingers required to trigger this gesture.<br>See **Fingers** for accepted values.<br>Default: **none**
      - **TriggerWhenThresholdReached** (bool) - Whether to trigger the gesture immediately after the specified threshold is reached.<br>Default: **false**
      - **TriggerOneActionOnly** (bool) - Whether to trigger only the first action that satisfies a condition.<br>Default: **false**<br>&nbsp;
      - **[Hold]** - Configuration for hold gestures.
        - **Threshold** (int) - In milliseconds.<br>Default: **0**
      - **[Pinch]** - Configuration for pinch gestures.
        - **Direction** (enum) - ``Contracting``, ``Expanding``
        - **Threshold** (float) - Should be >= 1.0 for expanding gestures and =< 1.0 for contracting gestures.<br>Default: **1.0**
      - **[Swipe]** - Configuration for swipe gestures.
        - **Direction** (enum) - ``Left``, ``Right``, ``Up``, ``Down``
        - **Threshold** (int) - Threshold in pixels for the X axis if **Direction** is ``Left`` or ``Right``, and Y axis if **Direction** is ``Up`` or ``Down``.<br>Default: **0**<br>&nbsp;
      - **[Conditions]** - At least one condition (or 0 if none specified) must be satisfied in order for this gesture to be triggered.
        - **[$ConditionId]** (int) - Unique ID for this condition.
          - **Negate** (bool) - If true, this condition will be satisfied only when none of its specified properties are.<br>Default: **false**
          - **WindowClassRegex** (string) - A regular expression executed on the currently focused window's resource class and resource name. If a match is not found for either, the condition will not be satisfied.<br>Default: **none**
          - **WindowState** (enum) - ``Fullscreen``, ``Maximized``. Values can be combined using the | separator, For example, ``Fullscreen|Maximized`` will match either fullscreen or maximized windows.<br>Default: **none**<br>&nbsp;
      - **[Actions]** - What do to when the gesture is triggered. Actions are executed in order as they appear in the configuration file.
        - **[$ActionId]** (int) - Unique ID for this action.
          - **Type** (enum)
            - ``Command`` - Run a command.
            - ``GlobalShortcut`` - Invoke a global shortcut.
            - ``KeySequence`` - Send keystrokes.<br>&nbsp;
          - **[Command]** - Configuration for the GlobalShortcut action.
            - **Command** (string) - The command to run.<br>Default: **none**
          - **[GlobalShortcut]** - Configuration for the GlobalShortcut action.
            - **Component** (string) - Run ``qdbus org.kde.kglobalaccel | grep /component`` for the list of components. Don't put the ``/component/`` prefix here.<br>Default: **none**
            - **Shortcut** (string) - Run ``qdbus org.kde.kglobalaccel /component/$component org.kde.kglobalaccel.Component.shortcutNames`` for the list of shortcuts.<br>Default: **none**
          - **[KeySequence]** - Configuration for the KeySequence action.
            - **Sequence** (string) - The key sequence to run. Case-sensitive. Invalid format will cause a crash (for now). For the full list of keys see [src/gestures/actions/keysequence.h](src/gestures/actions/keysequence.h).<br>Example: ``press LEFTCTRL,press T,release LEFTCTRL,release T``.<br>Default: **none**<br>&nbsp;
          - **[Conditions]** - Same as **[Gestures][\$Device][$GestureId][Conditions]**, but only applied to this action.

### Example
```
[Gestures][Touchpad][0] # Swipe 3 left
Type=Swipe
Fingers=3
TriggerWhenThresholdReached=true

[Gestures][Touchpad][0][Swipe]
Direction=Left
Threshold=10

[Gestures][Touchpad][0][Actions][0] # Firefox back
Type=KeySequence

[Gestures][Touchpad][0][Actions][0][Conditions][0]
WindowClassRegex=firefox

[Gestures][Touchpad][0][Actions][0][KeySequence]
Sequence=press LEFTCTRL,press LEFTBRACE,release LEFTCTRL,release LEFTBRACE


[Gestures][Touchpad][1] # Swipe 3 right
Type=Swipe
Fingers=3
TriggerWhenThresholdReached=true

[Gestures][Touchpad][1][Swipe]
Direction=Right
Threshold=10

[Gestures][Touchpad][1][Actions][0] # Firefox forward
Type=KeySequence

[Gestures][Touchpad][1][Actions][0][Conditions][0]
WindowClassRegex=firefox

[Gestures][Touchpad][1][Actions][0][KeySequence]
Sequence=press LEFTCTRL,press RIGHTBRACE,release LEFTCTRL,release RIGHTBRACE


[Gestures][Touchpad][2] # Swipe 3 down
Type=Swipe
Fingers=3
TriggerWhenThresholdReached=true

[Gestures][Touchpad][2][Swipe]
Direction=Down
Threshold=10

[Gestures][Touchpad][2][Actions][0] # Firefox refresh
Type=KeySequence

[Gestures][Touchpad][2][Actions][0][Conditions][0]
WindowClassRegex=firefox

[Gestures][Touchpad][2][Actions][0][KeySequence]
Sequence=press LEFTCTRL,press F5,release LEFTCTRL,release F5


[Gestures][Touchpad][3] # Swipe 4 down
Type=Swipe
Fingers=4
TriggerWhenThresholdReached=true

[Gestures][Touchpad][3][Swipe]
Direction=Down
Threshold=10

[Gestures][Touchpad][3][Actions][0] # Minimize window if not fullscreen and not maximized
Type=GlobalShortcut

[Gestures][Touchpad][3][Actions][0][Conditions][0]
Negate=true
WindowState=Fullscreen|Maximized

[Gestures][Touchpad][3][Actions][0][GlobalShortcut]
Component=kwin
Shortcut=Window Minimize

[Gestures][Touchpad][3][Actions][1] # Unmaximize window if maximized
Type=GlobalShortcut

[Gestures][Touchpad][3][Actions][1][Conditions][0]
WindowState=Maximized

[Gestures][Touchpad][3][Actions][1][GlobalShortcut]
Component=kwin
Shortcut=Window Maximize


[Gestures][Touchpad][4] # Swipe 4 up
Type=Swipe
Fingers=4
TriggerWhenThresholdReached=true

[Gestures][Touchpad][4][Swipe]
Direction=Up
Threshold=10

[Gestures][Touchpad][4][Actions][0] # Maximize window if not maximized
Type=GlobalShortcut

[Gestures][Touchpad][4][Actions][0][Conditions][0]
Negate=true
WindowState=Maximized

[Gestures][Touchpad][4][Actions][0][GlobalShortcut]
Component=kwin
Shortcut=Window Maximize


[Gestures][Touchpad][5] # Pinch 2 in
Type=Pinch
Fingers=2
TriggerWhenThresholdReached=true
TriggerOneActionOnly=true

[Gestures][Touchpad][5][Pinch]
Direction=Contracting
Threshold=0.9

[Gestures][Touchpad][5][Actions][0] # Lock session if no active window
Type=GlobalShortcut

[Gestures][Touchpad][5][Actions][0][Conditions][0]
WindowClassRegex=plasmashell

[Gestures][Touchpad][5][Actions][0][GlobalShortcut]
Component=ksmserver
Shortcut=Lock Session

[Gestures][Touchpad][5][Actions][1] # Close window, because of TriggerOneActionOnly=true, this will only trigger when the previous condition isn't satisfied
Type=GlobalShortcut

[Gestures][Touchpad][5][Actions][1][GlobalShortcut]
Component=kwin
Shortcut=Window Close


[Gestures][Touchpad][6] # Hold 3
Type=Hold
Fingers=3
TriggerWhenThresholdReached=true

[Gestures][Touchpad][6][Actions][0] # Launch krunner
Type=GlobalShortcut

[Gestures][Touchpad][6][Actions][0][GlobalShortcut]
Component=org_kde_krunner_desktop
Shortcut=_launch
```

# Gesture recognition issues
Before reporting any issues related to gesture recognition, run ``libinput debug-events`` as root and ensure the gesture is recognized properly. If it's not, there's nothing I can do.

Depending on the touchpad, 3 or 4-finger pinch gestures may sometimes be incorrectly interpreted as swipe gestures due to the touchpad only being able to track 2 fingers. As a workaround, move only 2 fingers in opposite directions. See https://wayland.freedesktop.org/libinput/doc/1.25.0/gestures.html#gestures-on-two-finger-touchpads for more information.

# Credits
- [KWin](https://invent.kde.org/plasma/kwin) - Gesture recognition, sending keystrokes