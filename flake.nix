{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    systems.url = "github:nix-systems/default";
    devenv.url = "github:cachix/devenv";
  };

  outputs = {
    self,
    nixpkgs,
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
        # TODO: upstream?
        vcpkgDeps = with pkgs; [
          pkg-config
          autoconf
          automake
          zip
          libtool
        ];

        nativePackages = [
          "qtbase"
          "qttools"
        ];
      in {
        default = pkgs.mkShell {
          nativeBuildInputs = with pkgs;
            [
              cmake
              makeWrapper
              ninja
              gcc
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
            export VCPKG_ROOT=$(realpath $(dirname $(readlink -f $(type -p vcpkg)))/../share/vcpkg)

            # set required environment variables for Qt
            setQtEnvironment=$(mktemp --suffix .setQtEnvironment.sh)
            makeShellWrapper "$(type -p sh)" "$setQtEnvironment" "''${qtWrapperArgs[@]}"
            sed "/^exec/d" -i "$setQtEnvironment"
            source "$setQtEnvironment"

            # TODO: upstream?
            # instead of fixing all the ports manually, we prompt the user to install them using Nix
            # in the future, a better solution would be to generate a nix derivation from
            # the vcpkg manifest

            newPortsDir=$HOME/.vcpkg/overlay-ports
            for port in ${pkgs.lib.concatStringsSep " " nativePackages}
            do
              portdir=$newPortsDir/$port
              mkdir -p $portdir
              echo 'set(VCPKG_POLICY_EMPTY_PACKAGE enabled)' > $portdir/portfile.cmake
              echo "message(WARNING Please install $port with Nix)" >> $portdir/portfile.cmake
              # We're going to manipulate the vcpkg.json here
              # We want to preserve the structure, but remove all dependencies
              ${pkgs.jq}/bin/jq 'walk(if type == "object" and has("dependencies") then .dependencies = [] else . end)' \
                "$VCPKG_ROOT/ports/$port/vcpkg.json" > "$newPortsDir/$port/vcpkg.json"
            done

            export VCPKG_OVERLAY_PORTS=$newPortsDir:$VCPKG_OVERLAY_PORTS
          '';
        };
      });
  };
}
