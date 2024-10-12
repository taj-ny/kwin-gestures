# KWin Gestures
Custom touchpad gestures for Plasma 6.

Tested on 6.1.5 and 6.2. X11 is not supported.

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

# Usage
1. Install the plugin.
2. Open the Desktop Effects page in System Settings.
3. Enable the *Gestures* effect.


# Configuration
There is currently no configuration UI and it may be a while before it's added. For now, you need to manually modify the ``~/.config/kwingesturesrc`` file. The format may change at any time until 1.0.0 is released.

Run ``qdbus org.kde.KWin /Effects org.kde.kwin.Effects.reconfigureEffect kwin_gestures`` or toggle the effect in system settings after updating the file.

### Configuration file structure

- **[Gestures]**
  - **[$device]** (enum) - Which device to add this gesture for (``Touchpad``).<br>
    - **[$name]** (string) - Unique name for the gesture.
      - **Type** (enum) - ``Hold``, ``Pinch``, ``Swipe``
      - **Fingers** (int) - Number of fingers required to trigger this gesture.<br>Sets ``MinimumFingers`` and ``MaximumFingers``.<br>Minimum value is 2 for hold and pinch gestures, 3 for swipe.<br>Maximum value is 4. 
      - **MinimumFingers** (int) - Minimum number of fingers required to trigger this gesture.<br>See **Fingers** for accepted values.
      - **MaximumFingers** (int) - Maximum number of fingers required to trigger this gesture.<br>See **Fingers** for accepted values.
      - **TriggerWhenThresholdReached** (bool) - Whether to trigger the gesture immediately after the specified threshold is reached.<br>Default: false
      - **WindowRegex** (string) - A regular expression executed on the currently focused window's resource class and resource name. If a match is not found for either, the gesture will be skipped. This allows you to create gestures only for specific applications.<br>Default: none<br>&nbsp;
      - **[Hold]** - Configuration for hold gestures.
        - **Threshold** (int) - In milliseconds.<br>Default: 0
      - **[Pinch]** - Configuration for pinch gestures.
        - **Direction** (enum) - ``Contracting``, ``Expanding``
        - **Threshold** (float) - Should be >= 1.0 for expanding gestures and =< 1.0 for contracting gestures.<br>Default: 1.0
      - **[Swipe]** - Configuration for swipe gestures.
        - **Direction** (enum) - ``Left``, ``Right``, ``Up``, ``Down``
        - **ThresholdX** (int) - Threshold for the X axis in pixels.<br>Only used if **Direction** is ``Left`` or ``Right``.<br>Default: 0
        - **ThresholdY** (int) - Threshold for the Y axis in pixels.<br>Only used if **Direction** is ``Up`` or ``Down``.<br>Default: 0<br>&nbsp;
      - **[Actions]** - What do to when the gesture is triggered. Actions are executed in order as they appear in the configuration file.
        - **[$name]** (string) - Unique name for the action.
          - **Type** (enum)
            - ``Command`` - Run a command.
            - ``GlobalShortcut`` - Invoke a global shortcut.
            - ``KeySequence`` - Send keystrokes.<br>&nbsp;
          - **[Command]** - Configuration for the GlobalShortcut action.
            - **Command** (string) - The command to run.
          - **[GlobalShortcut]** - Configuration for the GlobalShortcut action.
            - **Component** (string) - Run ``qdbus org.kde.kglobalaccel`` for a list of available components. Components start with the ``/component/`` prefix. Don't add the prefix here.
            - **Shortcut** (string) - See the ``~/.config/kglobalshortcutsrc`` file for a list of available shortcuts. The group name (or similar) is the component and the key (everything before *=*) is the shortcut.
          - **[KeySequence]** - Configuration for the KeySequence action.
            - **Sequence** (string) - The key sequence to run. Case-sensitive. Invalid format will cause a crash (for now).<br>Example: ``press LEFTCTRL,press T,release LEFTCTRL,release T``. For the full list of keys see [src/gestures/actions/keysequence.h](src/gestures/actions/keysequence.cpp).

### Example
```
[Gestures][Touchpad][Lock Screen]
Type=Pinch
Fingers=2
TriggerWhenThresholdReached=true
WindowRegex=plasmashell

[Gestures][Touchpad][Lock Screen][Pinch]
Direction=Contracting
Threshold=0.9

[Gestures][Touchpad][Lock Screen][Actions][0]
Type=GlobalShortcut

[Gestures][Touchpad][Lock Screen][Actions][0][GlobalShortcut]
Component=ksmserver
Shortcut=Lock Session


[Gestures][Touchpad][Close Window]
Type=Pinch
Fingers=2
TriggerWhenThresholdReached=true

[Gestures][Touchpad][Close Window][Pinch]
Direction=Contracting
Threshold=0.9

[Gestures][Touchpad][Close Window][Actions][0]
Type=GlobalShortcut

[Gestures][Touchpad][Close Window][Actions][0][GlobalShortcut]
Component=kwin
Shortcut=Window Close


[Gestures][Touchpad][Yakuake]
Type=Swipe
Fingers=4
TriggerWhenThresholdReached=true

[Gestures][Touchpad][Yakuake][Swipe]
Direction=Down
ThresholdY=10

[Gestures][Touchpad][Yakuake][Actions][0]
Type=GlobalShortcut

[Gestures][Touchpad][Yakuake][Actions][0][GlobalShortcut]
Component=yakuake
Shortcut=toggle-window-state


[Gestures][Touchpad][Konsole]
Type=Swipe
Fingers=3
TriggerWhenThresholdReached=true

[Gestures][Touchpad][Konsole][Swipe]
Direction=Up
ThresholdY=10

[Gestures][Touchpad][Konsole][Actions][0]
Type=Command

[Gestures][Touchpad][Konsole][Actions][0][Command]
Command=dolphin


[Gestures][Touchpad][Firefox Back]
Type=Swipe
Fingers=3
TriggerWhenThresholdReached=true
WindowRegex=firefox

[Gestures][Touchpad][Firefox Back][Swipe]
Direction=Left
ThresholdX=10

[Gestures][Touchpad][Firefox Back][Actions][0]
Type=KeySequence

[Gestures][Touchpad][Firefox Back][Actions][0][KeySequence]
Sequence=press LEFTCTRL,press LEFTBRACE,release LEFTCTRL,release LEFTBRACE


[Gestures][Touchpad][Firefox Forward]
Type=Swipe
Fingers=3
TriggerWhenThresholdReached=true
WindowRegex=firefox

[Gestures][Touchpad][Firefox Forward][Swipe]
Direction=Right
ThresholdX=10

[Gestures][Touchpad][Firefox Forward][Actions][0]
Type=KeySequence

[Gestures][Touchpad][Firefox Forward][Actions][0][KeySequence]
Sequence=press LEFTCTRL,press RIGHTBRACE,release LEFTCTRL,release RIGHTBRACE


[Gestures][Touchpad][Krunner]
Type=Hold
Fingers=2
TriggerWhenThresholdReached=true

[Gestures][Touchpad][Krunner][Hold]
Threshold=100

[Gestures][Touchpad][Krunner][Actions][0]
Type=GlobalShortcut

[Gestures][Touchpad][Krunner][Actions][0][GlobalShortcut]
Component=org_kde_krunner_desktop
Shortcut=_launch
```

# Credits
- [KWin](https://invent.kde.org/plasma/kwin)