{
  description = "Custom touchpad and touchscreen shortcuts";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/1925c603f17fc89f4c8f6bf6f631a802ad85d784";
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
