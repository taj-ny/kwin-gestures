# KWin Gestures
Mouse and touchpad gestures for Plasma 6.

X11 is currently not supported.

https://github.com/user-attachments/assets/2c16790a-869b-44f3-a760-590137293759

[peterfajdiga/kwin4_effect_geometry_change](https://github.com/peterfajdiga/kwin4_effect_geometry_change) was used for tile animations.

# Features
- Mouse gestures: press, stroke, swipe, wheel
  - Activated by pressing mouse button(s), keyboard modifier(s) or both
  - Mouse button events are blocked when a gesture is activated
  - Mouse buttons can still be used for normal clicks (a small delay is introduced)
  - Restrict gestures to specific screen edges/corners
  - Supports horizontal scrolling wheels
- Touchpad gestures: pinch, press, rotate, stroke, swipe
  - Supports 2-finger strokes and swipes by reinterpreting scroll events
- Actions: run command, simulate input (keyboard keys, mouse buttons, mouse movement), invoke Plasma global shortcut
  - Can be executed at a specific point of the gesture's lifecycle (begin, update, end, cancel), allowing for complex gestures like 3-finger window drag or alt+tab window switching without them being hard-coded
  - Update actions can repeat at a specified interval
- Application-specific gestures (based on window class)
- Bi-directional pinch/rotate/swipe gestures - change direction during a gesture and start executing different update actions - useful for gestures like volume control
- Keyboard modifiers - only activate a gesture when all specified modifiers (alt, control, meta, shift) are pressed
- Overrides built-in Plasma gestures if a custom one is activated
- Speed - only activate a gesture when performed at a specific speed
  - Two speed levels: slow, fast
  - Adjustable thresholds
- Stroke gestures - draw any shape
- Thresholds - only execute actions if the total distance fits within the specified range
  - Can be used to create short/long swipe gestures

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
  sudo pacman -S --needed base-devel git extra-cmake-modules qt6-tools kwin yaml-cpp
  ```
</details>

<details>
  <summary>Debian-based (KDE Neon, Kubuntu, Ubuntu)</summary>
  <br>

  ```
  sudo apt install git cmake g++ extra-cmake-modules qt6-tools-dev kwin-wayland kwin-dev libkf6configwidgets-dev gettext libkf6kcmutils-dev libyaml-cpp-dev libxkbcommon-dev
  ```
</details>

<details>
  <summary>Fedora 40, 41</summary>
  <br>

  ```
  sudo dnf install git cmake extra-cmake-modules gcc-g++ qt6-qtbase-devel kwin-devel kf6-ki18n-devel kf6-kguiaddons-devel kf6-kcmutils-devel kf6-kconfigwidgets-devel qt6-qtbase kf6-kguiaddons kf6-ki18n wayland-devel yaml-cpp yaml-cpp-devel libepoxy-devel
  ```
</details>

<details>
  <summary>openSUSE</summary>
  <br>

  ```
  sudo zypper in git cmake-full gcc-c++ kf6-extra-cmake-modules kguiaddons-devel kconfigwidgets-devel ki18n-devel kcmutils-devel "cmake(KF6I18n)" "cmake(KF6KCMUtils)" "cmake(KF6WindowSystem)" "cmake(Qt6Core)" "cmake(Qt6DBus)" "cmake(Qt6Quick)" "cmake(Qt6Widgets)" libepoxy-devel kwin6-devel yaml-cpp-devel libxkbcommon-devel
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

[Documentation](docs/index.md)

# Gesture recognition issues
Before reporting any issues related to gesture recognition, run ``libinput debug-events`` as root and ensure the gesture is recognized properly. If it's not, there's nothing I can do.

# Credits
- [KWin](https://invent.kde.org/plasma/kwin) - Gesture handling code (heavily modified and extended)
- [Strokognition](https://invent.kde.org/jpetso/strokognition), [wstroke](https://github.com/dkondor/wstroke), [easystroke](https://github.com/thjaeger/easystroke) - Stroke gestures