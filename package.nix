{ lib
, stdenv
, cmake
, extra-cmake-modules
, kwin
, wrapQtAppsHook
, qttools
, kglobalacceld
, yaml-cpp
}:

stdenv.mkDerivation rec {
  pname = "kwin-gestures";
  version = "0.4.0";

  src = ./.;

  nativeBuildInputs = [
    cmake
    extra-cmake-modules
    wrapQtAppsHook
  ];

  buildInputs = [
    kwin
    qttools
    kglobalacceld
    yaml-cpp
  ];

  meta = with lib; {
    description = "Custom touchpad gestures for Plasma 6";
    license = licenses.gpl3;
    homepage = "https://github.com/taj-ny/kwin-gestures";
  };
}
