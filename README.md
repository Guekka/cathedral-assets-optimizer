# Cathedral Assets Optimizer

Cathedral Assets Optimizer is a tool aiming to automate asset conversion and optimization for several Bethesda games, such as Skyrim and Skyrim Special Edition.

**This page is for the in-development, experimental version of the tool. Documentation is likely outdated.**

# Build instructions

For a Windows Ninja build with a preinstalled Qt MSVC 2022 SDK, open a Visual Studio 2022 Developer PowerShell and set `QT_ROOT_DIR` to the SDK root. Set `VCPKG_ROOT` to the repository's vcpkg checkout, then run:

```powershell
$env:QT_ROOT_DIR = 'C:\Qt\6.8.3\msvc2022_64'
$env:VCPKG_ROOT = "$PWD\vcpkg"
cmake --preset ninja-windows-prebuilt-qt
cmake --build build/ninja-windows-prebuilt-qt
```

The regular `ninja-windows` and `ninja-linux` presets remain vcpkg-based; use a `*-prebuilt-qt` preset when Qt comes from an SDK or Nix.

For a Linux build using the Qt packages provided by the Nix dev shell:

```sh
nix develop
cmake --preset ninja-linux-prebuilt-qt
cmake --build build/ninja-linux-prebuilt-qt
```

All prebuilt-Qt presets use the checked-in `cmake/prebuilt-qt-ports` overlay. Its vcpkg version is a shim marker; the actual Qt version comes from the SDK or Nix package.

See [the wiki](https://gitlab.com/G_ka/sse-assets-optimiser/wikis/Build-instructions).

# CI distributions

GitHub Actions uploads an executable-only artifact for successful builds. A manual workflow run with `package_only` enabled packages and reports the Windows bundle without publishing it. Pushing a `v*` tag creates a GitHub Release with a Windows bundle containing the Qt runtime DLLs and plugins, uploads that bundle as a new version of the CAO7 optional file on Nexus Mods, and attaches an optional Wine/Proton `icuuc.dll` forwarder zip. Configure the repository secret `NEXUSMODS_API_KEY` before tagging a release. For Wine or Proton, extract the forwarder's `icuuc.dll` beside the executable; do not use it for native Windows.

# Features and use instructions

See [the NexusMods page](https://www.nexusmods.com/skyrimspecialedition/mods/23316).

# Credits

A lot of people helped me throughout the development of this tool, and I would like to thank them all.
If you feel like you should be in this list, please let me know, as I'm writing this list way after the fact.

Some tools listed here might be no longer used in the project, but I still want to thank their authors for their work.

- [Zilav](https://github.com/zilav), for his assistance and [BSArch](https://github.com/TES5Edit/TES5Edit/tree/dev/Tools/BSArchive)
- [Ousnius](https://github.com/ousnius), for his assistance and [nifly](https://github.com/ousnius/nifly)
- Microsoft, for [DirectXTex](https://github.com/Microsoft/DirectXTex) (and making it Linux-compatible!)
- [Figment](figment), for [hkxcmd](https://github.com/figment/hkxcmd)
- [Deorder](https://github.com/deorder/), for his assistance and [Libbsarch](https://github.com/deorder/libbsarch)
- [Francesc M.](francescmm), for [QLogger](https://github.com/francescmm/QLogger)
- [Feles Noctis](https://ko-fi.com/felesnoctis), [Hishy](https://www.nexusmods.com/skyrimspecialedition/users/3259968), [Alsa](https://github.com/alsa64), [Aerisarn](https://github.com/aerisarn), and many others, for tests and advice
