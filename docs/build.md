# Build CAO from source

## Windows

These instructions use the repository's `vs2022-windows` CMake preset.

### Requirements

- Windows 10 or later
- Visual Studio 2022 with the **Desktop development with C++** workload and a Windows SDK
- CMake 3.21 or later
- Git
- vcpkg

CAO's manifest installs its dependencies, including Qt 6. You do not need to install Qt separately
or pass Qt directory options to CMake.

### Install vcpkg

Clone and bootstrap vcpkg, then set the `VCPKG_ROOT` environment variable to its directory. For example,
in PowerShell:

```powershell
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
& C:\vcpkg\bootstrap-vcpkg.bat
$env:VCPKG_ROOT = "C:\vcpkg"
```

Set `VCPKG_ROOT` as a persistent user environment variable if you want to use it from future terminals.
Open a new terminal after changing persistent environment variables.

### Configure and build

Clone the CAO repository and configure it with the Windows preset:

```powershell
git clone https://github.com/Guekka/cathedral-assets-optimizer.git
cd cathedral-assets-optimizer
cmake --preset vs2022-windows
cmake --build build --config Debug
```

The first configure downloads the dependencies declared by the vcpkg manifest and may take some time.
The preset generates a Visual Studio solution in `build`; you can open it in Visual Studio instead of
building from the command line.

The executable target is `Cathedral_Assets_Optimizer`. To build only that target:

```powershell
cmake --build build --config Debug --target Cathedral_Assets_Optimizer
```

### Troubleshooting

- If CMake cannot find vcpkg, confirm `VCPKG_ROOT` points to the vcpkg checkout containing
  `scripts/buildsystems/vcpkg.cmake`, then configure from a new terminal.
- If configuration reports a missing package, check that vcpkg completed its manifest install and
  review the first package error in the configure output.
- Do not pass the old `Qt5_DIR` or `Qt5LinguistTools_DIR` options; this version uses Qt 6 through
  vcpkg.

## Linux with Nix

The repository provides a Nix flake with the compiler, CMake, Ninja, vcpkg, Qt 6, and the supporting
build tools. Use the `ninja-linux-prebuilt-qt` preset when using Qt from the Nix development shell; it
uses the `x64-linux` vcpkg triplet and the Qt provided by Nix.

### Requirements

- An x86_64 Linux system
- Nix with the `nix-command` and `flakes` experimental features enabled
- Git

You do not need to install CMake, Ninja, vcpkg, Qt, or the compiler separately; the development shell
provides them.

### Configure and build

Clone the repository if you have not already, then run these commands from its root:

```sh
nix develop
```

This opens a shell with the flake's development environment active. From that shell, configure and
build CAO:

```sh
cmake --preset ninja-linux-prebuilt-qt
cmake --build build/ninja-linux-prebuilt-qt
```

The CMake preset configures a Debug build with Ninja; the first configure/build may download
dependencies through vcpkg and take some time. The executable target is `Cathedral_Assets_Optimizer`.
Nix supplies Qt while vcpkg installs the remaining manifest dependencies.

The Linux preset enables AddressSanitizer, UndefinedBehaviorSanitizer, and coverage instrumentation.

If you use direnv, the repository's `.envrc` already enters this development shell. After installing
and configuring direnv for your shell, run `direnv allow` from the repository root.

If flakes are not enabled in your Nix configuration, enable the `nix-command` and `flakes` features
for this invocation:

```sh
nix --extra-experimental-features "nix-command flakes" develop
```

### Troubleshooting

- If Nix reports that `nix-command` or `flakes` is experimental and disabled, enable both features in
  your Nix configuration and start a new shell.
- If CMake cannot find vcpkg or Qt, make sure you entered the flake development shell and use the
  `ninja-linux-prebuilt-qt` preset. The shell sets up `VCPKG_ROOT`, `Qt6_DIR`, Qt's runtime
  environment, and the prebuilt-Qt overlay ports.
