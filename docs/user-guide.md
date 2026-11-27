# Cathedral Assets Optimizer User Guide

This guide covers the current development interface. Names and options may change between versions.

## Before processing

CAO works on the directory you select and can modify or remove files there. It does not create a backup
of your mod. Keep a separate copy or use your mod manager's backup/versioning features before running
an operation that changes files.

Choose a target game that matches the assets you are processing. In Quick tasks, select the game
directly; in Customize, select a profile with the matching target game. Game profiles select
game-specific formats and defaults, so using the wrong game can produce unsuitable results.

## Choose a workspace

- **Quick tasks** provides a short path for preparing Skyrim LE/SSE assets with CAO's recommended
  settings. Choose the target game without changing your saved profile.
- **Customize** exposes profiles, operation settings, and file rules for users who need more control.

Use Quick tasks when the recommended asset-preparation workflow fits. Use Customize when you need to
reuse settings, choose individual operations, or make file-specific exceptions. Hover over controls
with a help cursor to read their tooltips.

## Choose what to process

Select **One mod** to process the directory in the input path. Select **Several mods** to choose a
parent directory: CAO processes each immediate subdirectory as a separate mod. Check that the selected
directory has the layout you intend before running.

Use **Open Directory** to choose the input path. The path can also be entered directly in the field.
The input path and **One mod/Several mods** choice are shared across workspaces; changing profiles
does not change them.

## Preview with Dry run

Click **Preview (Dry run)** to see the planned processing steps without modifying files. Click
**Run** to process the selected input. Preview is not a backup; keep a separate copy or use your mod
manager's backup/versioning features before running.

## Profiles and setting scope

A profile stores the target game and reusable processing settings. In **Customize**, use **Manage**
beside the profile selector to create, remove, import, or export profiles. Select the profile whose
target game and settings match the mod. The Skyrim target in Quick tasks is independent of the
selected profile.

The selected profile is saved with CAO's settings. Export a profile if you want to share or keep a
separate copy of its configuration.

In **Customize**, the profile controls identify the game and profile. The scope label above the
operation tabs and the selection in the left-hand file-rules panel show which file settings you are
editing:

- **Profile** selects the complete game profile.
- **Default file settings** are selected with **Everything else (default)** and are the fallback
  used when no file rule matches. If there are no active rules, these settings apply to all files.
- Selecting a file rule means the operation tabs edit that rule's settings; they apply only when it
  is the first matching rule.

The **BSA (Profile-wide)** module configures BSA processing for the whole profile, even while a file
rule is selected. The other operation settings belong to the default settings or selected rule, as
shown by the scope label.

## File rules

File rules let **Customize** apply different settings to relative paths. The left-hand **File rules**
panel keeps rule priority and active settings scope visible beside the operation tabs. The first
matching rule supplies the settings, and **Default file settings** are the fallback.

The sample-path and real-input impact previews are read-only and distinct from **Dry run**. For match
types, rule priority, setting scope, and preview details, see the [file rules guide](patterns.md).

## BSA options

In **Customize**, **BSA (Profile-wide)** controls archive processing for the profile:

- **Extract BSAs** extracts archive contents. Existing loose files are not overwritten.
- **Create BSAs** packs eligible loose files into archives.
- **Do not remove packed files** keeps the loose source files after packing. If unchecked, CAO may
  remove files that were successfully packed.
- **Do not compress files** disables compression for created archives.

The **Pack file** setting in the **BSA** module is a file setting: in Customize, it belongs to the
selected rule or the default fallback. It controls whether those files are eligible for packing.
Packing has no effect unless **Create BSAs** is enabled in **BSA (Profile-wide)**.

## Progress and logs

The progress window shows processing status and a log. Use its log-level selector to show Verbose,
Info, or Error entries. **Open log file** opens the current log file in the system's default viewer.
CAO rotates log files to keep them from growing indefinitely; older log segments are in the same log
directory.
