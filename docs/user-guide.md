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
  and SSE profiles, with a limited set of options.
- **Medium** adds BSA and texture settings, along with profile selection.
- **Advanced** exposes the full set of modules, profiles, and file rules.

Quick Optimize uses CAO's built-in defaults and does not expose file-rule controls. Medium and
Advanced modes are intended for users who need to customize what CAO does. Hover over controls with a
help cursor to read their tooltips.

## Choose what to process

Select **One mod** to process the directory in the input path. Select **Several mods** to choose a
parent directory: CAO processes each immediate subdirectory as a separate mod. Check that the selected
directory has the layout you intend before running.

Use **Open Directory** to choose the input path. The path can also be entered directly in the field.

## Preview with Dry run

Enable **Dry run** and click **Run** to see which files would be affected without performing their
optimizations. Review the progress log before running with Dry run disabled. A dry run is a preview,
not a backup.

## Profiles and setting scope

A profile stores the target game and the settings used for processing. In Medium and Advanced modes,
use **Manage** beside the profile selector to create, remove, import, or export profiles. Select the
profile whose target game and settings match the mod.

The selected profile is saved with CAO's settings. Export a profile if you want to share or keep a
separate copy of its configuration.

In Advanced mode, the controls above the modules show which settings you are editing:

- **Profile** selects the complete game profile.
- **Default file settings** are the fallback used when no file rule matches. If there are no active
  rules, these settings apply to all files.
- **For matching files** means the settings belong to the selected rule and apply only when it is the
  first matching rule.

The **BSA (Profile-wide)** module is an exception to the file-settings selector: it configures BSA
processing for the whole profile, even while a file rule is selected. Other file settings belong to
the default settings or the selected rule, as shown by the scope label.

Medium mode edits **Default file settings**. Any file rules already in the profile still override
those defaults for matching files; Medium shows how many are active. Switch to Advanced to review or
edit the rules.

## File rules

File rules let Advanced mode apply different settings to different relative file paths. Open **Manage
rules** beside the file-settings selector to create, edit, reorder, or remove rules.

1. Choose **New rule...** and select the recommended folder-and-extension match, or an advanced glob
   or regular expression.
2. Optionally name the rule and choose which existing settings to copy as a starting point.
3. Drag rules to change their priority. Select a rule in the main window to edit its settings.
4. Use the sample-path preview at the bottom of the manager to check which rule would apply to a
   relative path.

Rules are checked from top to bottom, and the first matching rule supplies the file settings. Rules
do not merge settings. The **Everything else (default)** entry is the fallback for files that match
no rule; it can be selected to edit **Default file settings**, but it is not managed as a rule.

For examples and details about matching, scope, and priority, see the [file rules guide](patterns.md).

## BSA options

In Medium or Advanced mode, **BSA (Profile-wide)** controls archive processing for the profile:

- **Extract BSAs** extracts archive contents. Existing loose files are not overwritten.
- **Create BSAs** packs eligible loose files into archives.
- **Do not remove packed files** keeps the loose source files after packing. If unchecked, CAO may
  remove files that were successfully packed.
- **Do not compress files** disables compression for created archives.

The **Pack file** setting in the **BSA** module is a file setting: in Advanced mode, it belongs to the
selected rule or the default fallback. It controls whether those files are eligible for packing.
Packing has no effect unless **Create BSAs** is enabled in **BSA (Profile-wide)**.

## Progress and logs

The progress window shows processing status and a log. Use its log-level selector to show Verbose,
Info, or Error entries. **Open log file** opens the current log file in the system's default viewer.
CAO rotates log files to keep them from growing indefinitely; older log segments are in the same log
directory.
