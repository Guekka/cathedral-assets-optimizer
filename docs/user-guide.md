# Cathedral Assets Optimizer User Guide

This guide covers the current development interface. Names and options may change between versions.

## Before processing

CAO works on the directory you select and can modify or remove files there. It does not create a backup
of your mod. Keep a separate copy or use your mod manager's backup/versioning features before running
an operation that changes files.

Choose the game profile that matches the assets you are processing. Game profiles select game-specific
formats and defaults; using the wrong game can produce unsuitable results.

## Choose a mode

Use **Options > Change level** to choose how much of CAO's configuration to show:

- **Quick Optimize** uses default settings for common Skyrim LE/SSE porting tasks. It offers the SLE
  and SSE profiles.
- **Medium** adds BSA and texture settings.
- **Advanced** exposes the full set of modules, profiles, and per-file patterns.

Medium and Advanced modes are intended for users who need to customize what CAO does. Hover over
controls with a help cursor to read their tooltips.

## Choose what to process

Select **One mod** to process the directory in the input path. Select **Several mods** to choose a
parent directory: CAO processes each immediate subdirectory as a separate mod. Check that the selected
directory has the layout you intend before running.

Use **Open Directory** to choose the input path. The path can also be entered directly in the field.

## Preview with Dry run

Enable **Dry run** and click **Run** to see which files would be affected without performing their
optimizations. Review the progress log before running with Dry run disabled. A dry run is a preview,
not a backup.

## Profiles

A profile stores the target game and the settings used for processing. In Medium and Advanced modes,
use **Manage** beside the profile selector to create, remove, import, or export profiles. Select the
profile whose target game and settings match the mod.

The selected profile is saved with CAO's settings. Export a profile if you want to share or keep a
separate copy of its configuration.

## Patterns and per-file settings

Patterns let Advanced mode apply different settings to different relative file paths:

1. Select a pattern in the main window. The settings shown in the modules belong to that pattern.
2. Open **Manage** beside the pattern selector to create, edit, reorder, or remove patterns.
3. New patterns can use a glob pattern or a regular expression and can start with the settings of an
   existing pattern.
4. Double-click a pattern to edit its text. Its type (glob or regular expression) is chosen when it is
   created and cannot be changed later.
5. Drag patterns to reorder them.

CAO uses the first matching pattern in the manager list, from top to bottom. Put more specific patterns
before broader ones. The default `*` pattern supplies settings for files not matched by another pattern.
Changing settings while one pattern is selected changes that pattern, not every pattern in the profile.

For examples and details about matching and priority, see the [patterns guide](patterns.md).

## BSA options

In Medium or Advanced mode, the **BSA (General)** module controls archive processing:

- **Extract BSAs** extracts archive contents. Existing loose files are not overwritten.
- **Create BSAs** packs eligible loose files into archives.
- **Do not remove packed files** keeps the loose source files after packing. If unchecked, CAO may
  remove files that were successfully packed.
- **Do not compress files** disables compression for created archives.

In Advanced mode, the **Pack file** setting on a pattern controls whether matching files are eligible
for packing. Packing has no effect unless **Create BSAs** is enabled.

## Progress and logs

The progress window shows processing status and a log. Use its log-level selector to show Verbose,
Info, or Error entries. **Open log file** opens the current log file in the system's default viewer.
CAO rotates log files to keep them from growing indefinitely; older log segments are in the same log
directory.
