{
  description = "Custom touchpad and touchscreen shortcuts";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/5df43628fdf08d642be8ba5b3625a6c70731c19c";
    utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, ... }@inputs: inputs.utils.lib.eachSystem [
    "x86_64-linux" "aarch64-linux"
  ] (system: let
    pkgs = import nixpkgs {
      inherit system;
    };
  in rec {
    packages.default = pkgs.kdePackages.callPackage ./package.nix { };

    devShells.default = pkgs.mkShell {
      inputsFrom = [ packages.default ];
    };
  });
}
