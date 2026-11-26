# Patterns

Patterns are available in Advanced mode. They let a profile apply different per-file settings to
different relative paths within a mod.

## Match files

Patterns can be glob patterns or regular expressions. For example, if a mod contains:

```text
main.dds
landscape.dds
normalMap.dds
```

the glob pattern `*.dds` matches all three files. A pattern is matched against the file's normalized
relative path, not just its name. Regular expressions are case-insensitive and must match the complete
relative path. Regex patterns are shown in bold in the Patterns Manager.

## Add and edit patterns

Use **Manage** beside the pattern selector in the main window:

1. Choose **New**, enter the pattern text, and choose **Glob** or **Regex**.
2. Choose an existing pattern as the starting settings for the new one.
3. Double-click an item in the list to edit its pattern text. The pattern type cannot be changed after
   creation.
4. Drag items to change their order, or select an item and choose **Remove** to delete it.

The default `*` pattern is the fallback settings for files that do not match another pattern. It can be
selected in the main window, but is not listed for removal in the Patterns Manager.

## Priority and settings

CAO checks patterns in the order shown in the manager, from top to bottom, and uses the first match.
There is no merging of settings from multiple matching patterns. Put specific patterns above broad
patterns so they take priority. For example, place `*.dds` above `*` to give DDS files their own
settings while `*` provides a fallback for other files.

The modules display and edit the settings for the currently selected pattern. A new pattern begins
with a copy of the chosen base pattern's settings; after creation, changes to one pattern do not
automatically change the others.
