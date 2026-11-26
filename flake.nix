{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.11";
    systems.url = "github:nix-systems/default";
    devenv.url = "github:cachix/devenv";
  };

  outputs = {
    self,
    nixpkgs,
    systems,
    ...
  }: let
    forEachSystem = nixpkgs.lib.genAttrs (import systems);
  in {
    devShells =
      forEachSystem
      (system: let
        pkgs = nixpkgs.legacyPackages.${system};
        # TODO: upstream?
        vcpkgDeps = with pkgs; [
          pkg-config
          autoconf
          automake
          zip
          libtool
        ];
      in {
        default = pkgs.mkShell {
          nativeBuildInputs = with pkgs;
            [
              cmake
              makeWrapper
              ninja
              gcc14
              vcpkg

              # profiling
              linuxPackages_latest.perf

              qt6.qtbase
              qt6.qttools
              qt6.qtwayland
              qt6.wrapQtAppsHook
            ]
            ++ vcpkgDeps;

          shellHook = ''
            export VCPKG_ROOT=${pkgs.vcpkg}/share/vcpkg
            export Qt6_DIR=${pkgs.qt6.qtbase}/lib/cmake/Qt6
            export VCPKG_OVERLAY_PORTS=${self}/cmake/ports:${self}/cmake/prebuilt-qt-ports''${VCPKG_OVERLAY_PORTS:+:$VCPKG_OVERLAY_PORTS}

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
