{
  description = "Custom touchpad and touchscreen shortcuts";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/d70bd19e0a38ad4790d3913bf08fcbfc9eeca507";
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
