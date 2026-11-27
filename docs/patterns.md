# File rules

File rules (called patterns in older CAO versions) let a profile apply different file settings to
different relative paths within a mod. You create and edit them in Advanced mode. Existing rules
continue to apply in Medium mode, where you can edit only the default file settings.

## Choose what to match

The recommended **Folder and file extension** match lets you combine a folder and an extension. If
both are provided, a file must match both conditions. Either condition can be left blank.

For example, a folder of `textures/interface` and an extension of `dds` matches:

```text
textures/interface/menu.dds
textures/interface/icons/menu.dds
```

Folder matching includes subfolders and can match the folder path within a longer relative path.
Extension matching is case-insensitive. Use a folder condition alone to match all file types in that
folder, or an extension alone to match that file type in any folder.

**Glob pattern** and **Regular expression** are available as advanced match types. They are matched
against the complete normalized relative file path. If you want to match a folder and all of its
subfolders without writing an expression, use the recommended folder-and-extension match.

## Create and organize rules

In Advanced mode, choose **Manage rules** beside the file-settings selector:

1. Choose **New rule...**, select a match type, and optionally give the rule a descriptive name.
2. Choose **Copy settings from** to start with the default settings or another rule's settings.
3. Select a rule and choose **Edit rule...** to change its name or match conditions.
4. Drag rules to change their order, or choose **Remove rule** to delete one.

The **Everything else (default)** entry is the fallback for paths that do not match a file rule. It
appears in the main window's selector, where it is labeled **Default file settings**, but is not listed
in the manager and cannot be removed.

## Priority and settings scope

CAO checks rules from top to bottom and applies the settings from the first matching rule. If more
than one rule matches a file, later matches do not contribute settings. Put narrower rules above
broader ones so they take priority.

In Advanced mode, **For matching files** identifies settings for the selected rule. They apply only
when that rule is the first match. **Default file settings** apply only when no rule matches. Medium
mode always edits the default settings; active rules continue to override them for matching files.

**BSA (Profile-wide)** settings are separate from file rules and apply to the profile as a whole,
even when a rule is selected.

## Check a rule

The file rules manager includes a sample-path preview. Enter a relative path, such as
`textures/interface/menu.dds`, to see whether the selected rule matches and whether an earlier rule
or the default settings would apply.
