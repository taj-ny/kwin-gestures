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
  version = "0.2.0";

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
    description = "Custom touchpad and touchscreen shortcuts";
    license = licenses.gpl3;
    homepage = "https://github.com/taj-ny/kwin-custom-shortcuts";
  };
}
