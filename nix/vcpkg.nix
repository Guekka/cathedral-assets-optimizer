{
  autoconf,
  automake,
  bash,
  cacert,
  cmake,
  cmrc,
  coreutils,
  curl,
  fetchFromGitHub,
  fmt_9,
  gcc,
  git,
  glibc,
  gnumake,
  gperf,
  gzip,
  jq,
  lib,
  libtool,
  makeWrapper,
  meson,
  m4,
  ninja,
  perl,
  pkg-config,
  python3,
  runtimeShell,
  stdenv,
  writeText,
  zip,
  zstd,
}
: let
  runtimeDeps = [
    autoconf
    automake
    bash
    cacert
    coreutils
    curl
    cmake
    gcc
    git
    gnumake
    gzip
    meson
    ninja
    perl
    pkg-config
    python3
    zip
    zstd
  ];
in
  stdenv.mkDerivation rec {
    pname = "vcpkg";
    version = "b18b17865cfb6bd24620a00f30691be6775abb96";

    src = fetchFromGitHub {
      owner = "microsoft";
      repo = "vcpkg-tool";
      rev = "bedcba5172f5e4b91caac660ab7afe92c27a9895";
      hash = "sha256-XgYCa9aiG8lOraClfsFCk7U1qxvsfsIKbcBd2xBcNSw=";
    };

    vcpkg_src = fetchFromGitHub {
      owner = "microsoft";
      repo = "vcpkg";
      rev = "5d2a0a9814db499f6ba2e847ca7ab5912badcdbf";
      hash = "sha256-UPThRUq8N2nmUoFMUgfLqu1JM3pjCz6mVdR11Iig4kE=";
    };

    nativeBuildInputs = [
      cmake
      cmrc
      fmt_9
      makeWrapper
    ];

    cmakeFlags = [
      "-DVCPKG_DEPENDENCY_EXTERNAL_FMT=ON"
      "-DVCPKG_DEPENDENCY_CMAKERC=ON"
    ];

    vcpkgScript = let
      out = placeholder "out";
    in ''
      #!${runtimeShell}
      vcpkg_hash=$(echo -n "${out}" | sha256sum | cut -f1 -d ' ')
      vcpkg_root_path="$HOME/.local/vcpkg/roots/$vcpkg_hash"
      if [[ ! -d $vcpkg_root_path ]]; then
        mkdir -p $vcpkg_root_path
        ln -s ${out}/share/vcpkg/{docs,ports,scripts,triplets,versions,LICENSE.txt} $vcpkg_root_path/
        ln -s ${out}/bin/vcpkg $vcpkg_root_path/
        touch $vcpkg_root_path/.vcpkg-root # need write access
      fi

      if [[ "$1" == "--root-for-nix-usage" ]]; then
        echo "$vcpkg_root_path"
        exit 0
      fi

      export VCPKG_FORCE_SYSTEM_BINARIES=1
      export VCPKG_ROOT="$vcpkg_root_path"
      export VCPKG_DOWNLOADS="$vcpkg_root_path/downloads"
      exec ${out}/share/vcpkg/vcpkg "$@"
    '';

    passAsFile = ["vcpkgScript"];

    # No need to write all dependencies, only the main package
    nativePackages = [
      "gettext"
      "qt"
      "qt5"
    ];

    postInstall = ''
      mkdir -p $out/share/vcpkg

      cp --preserve=mode -r ${vcpkg_src}/{docs,ports,scripts,triplets,versions,LICENSE.txt} $out/share/vcpkg
      mv $out/bin/vcpkg $out/share/vcpkg/vcpkg
      cp $vcpkgScriptPath $out/bin/vcpkg
      chmod +x $out/bin/vcpkg
      touch $out/share/vcpkg/vcpkg.disable-metrics
    '';

    postFixup = ''
      # instead of fixing all the ports manually, we prompt the user to install them using Nix
      # in the future, a better solution would be to generate a nix derivation
      for port in ${lib.concatStringsSep " " nativePackages}
      do
        portfile=$out/share/vcpkg/ports/$port/portfile.cmake
        echo 'set(VCPKG_POLICY_EMPTY_PACKAGE enabled)' > $portfile
        echo "message(WARNING Please install $port with Nix)" >> $portfile

        # We're going to manipulate the vcpkg.json here
        # We want to preserve features, but remove all dependencies
        manifest=$out/share/vcpkg/ports/$port/vcpkg.json
        ${jq}/bin/jq 'walk(if type == "object" and has("dependencies") then .dependencies = [] else . end)' \
          $manifest > result.json
        mv result.json $manifest
      done

      wrapProgram $out/share/vcpkg/vcpkg --set PATH ${lib.makeBinPath runtimeDeps}
    '';
  }
