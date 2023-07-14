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
        default = devenv.lib.mkShell {
          inherit inputs pkgs;
          modules = [
            {
              packages = with pkgs; [
                cmake
                ninja
                pkg-config
                gcc
                vcpkg
                libsForQt5.qt5.qtbase
                libsForQt5.qt5.qttools
              ];

              enterShell = ''
                export VCPKG_INSTALLATION_ROOT=$(vcpkg --root-for-nix-usage)
              '';
            }
          ];
        };
      });
  };
}
