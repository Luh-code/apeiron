{
  description = "Apeiron project flake";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    lib-apeiron-logger.url = "./apeiron-logger";
    lib-apeiron-core.url = "./apeiron-core";
  };

  outputs = { self, nixpkgs, flake-utils, lib-apeiron-logger, lib-apeiron-core }: 
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        packages = {
          lib-apeiron-logger = lib-apeiron-logger.packages.${system}.default;
          lib-apeiron-core = lib-apeiron-core.packages.${system}.default;
          default = pkgs.stdenv.mkDerivation {
            name = "main-apeiron";
            src = ./.;
            buildInputs = [
              lib-apeiron-logger.packages.${system}.default
              #lib-apeiron-core.packages.${system}.default
            ];
            # build instructions
          };
        };

        devShells.default = pkgs.mkShell {
          inputsFrom = [
            self.packages.${system}.lib-apeiron-logger
            #self.packages.${system}.lib-apeiron-core
            self.packages.${system}.default
          ];
        };
      });
}
