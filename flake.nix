{
  description = "Custom touchpad and touchscreen shortcuts";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, ... }@inputs: inputs.utils.lib.eachSystem [
    "x86_64-linux" "aarch64-linux"
  ] (system: let
    pkgs = import nixpkgs {
      inherit system;
    };
  in rec {
    packages = {
      default = pkgs.kdePackages.callPackage ./package.nix { };
      debug = pkgs.kdePackages.callPackage ./package.nix {
        debug = true;
      };
    };

    devShells.default = pkgs.mkShell {
      inputsFrom = [ packages.debug ];
    };
  });
}
