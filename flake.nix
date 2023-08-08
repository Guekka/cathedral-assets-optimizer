{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-23.05";
    vcpkg-repo.url = "github:Guekka/nixpkgs/vcpkg";
    systems.url = "github:nix-systems/default";
    devenv.url = "github:cachix/devenv";
  };

  outputs = {
    self,
    nixpkgs,
    vcpkg-repo,
    devenv,
    systems,
    ...
  } @ inputs: let
    forEachSystem = nixpkgs.lib.genAttrs (import systems);
  in {
    devShells =
      forEachSystem
      (system: let
        pkgs = nixpkgs.legacyPackages.${system};
        vcpkg = vcpkg-repo.legacyPackages.${system}.vcpkg;
      in {
        default = pkgs.mkShell {
              nativeBuildInputs = with pkgs; [
                cmake
                makeWrapper
                ninja
                gcc
                vcpkg

                # profiling
                linuxPackages_latest.perf

                qt6.qtbase
                qt6.qttools
                qt6.wrapQtAppsHook
              ];

              shellHook = ''
                export VCPKG_ROOT="$HOME/.local/share/vcpkg/root"

                # set required environment variables for Qt
                setQtEnvironment=$(mktemp --suffix .setQtEnvironment.sh)
                makeShellWrapper "$(type -p sh)" "$setQtEnvironment" "''${qtWrapperArgs[@]}"
                sed "/^exec/d" -i "$setQtEnvironment"
                source "$setQtEnvironment"
              '';
        };
      });
  };
}
