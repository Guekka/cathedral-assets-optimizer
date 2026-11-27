# Port a mod from Skyrim LE to SSE

CAO's Quick tasks workspace includes a recommended workflow for common Skyrim LE-to-SSE asset
preparation. Results depend on the mod; always keep a backup and test the converted mod.

## Prepare

1. Install CAO and the mod you want to convert. Mod Organizer 2 or another mod manager makes it easier
   to keep the original and converted files separate.
2. Keep a backup. If possible, make a copy of the mod and convert the copy.
3. If the mod contains a plugin, install the Skyrim Special Edition Creation Kit and the masters that
   plugin requires. Follow the current installation instructions for any Creation Kit fixes you use,
   such as [CK Fixes](https://www.nexusmods.com/skyrimspecialedition/mods/20061) or the
   [Multiple Masters fix](https://www.nexusmods.com/skyrimspecialedition/mods/2786).

## Process assets with CAO

1. Start CAO and open **Quick tasks**.
2. Select **Skyrim SE (2016)** as the target game.
3. Choose **One mod** and select the copied mod directory. Use **Several mods** only if you intend to
   process each immediate subdirectory of a parent directory as its own mod.
4. Click **Preview (Dry run)** to review planned processing steps; this does not back up your files.
5. Click **Run** and wait for processing to finish. Review the progress log for errors.

Quick tasks uses CAO's defaults for common porting work. If you need to adjust individual asset
settings, open **Customize** and read the module tooltips. The **Recommended** mesh optimization
targets mesh problems that are likely to crash the game; **Extensive** optimization is more aggressive
and is intended only when the recommended option does not address the problem.

## Handle a plugin, if present

Asset conversion does not necessarily convert a mod's plugin. If the plugin needs to be resaved for
SSE, run the SSE Creation Kit through your mod manager, select the plugin in the Data window, mark it
active, then save and close the Creation Kit. Make sure required master files are available to the
Creation Kit.

Afterward, test the mod in game. Use xEdit to inspect plugin warnings or errors and follow the relevant
modding guide before making further changes; cleaning a plugin is not a required step for every mod.

## If something goes wrong

- **Black faces:** Check that the mod's FaceGen files are present and that the mod load order is
  correct. If needed, follow a current guide to regenerate FaceGen for the affected NPCs.
- **Crashes on launch or in game:** Check CAO's log and confirm the plugin and its masters were
  converted and installed correctly. A crash can have causes beyond asset conversion.
- **Mesh or visual problems:** Identify the affected mesh first. CAO's **Extensive** mesh option is
  more aggressive and has a lower chance of success; keep a backup and test after any change.

For Creation Kit and asset-conversion issues, use current Skyrim SE modding guides because fixes and
tool requirements can change over time.

## Credits

The original porting instructions were contributed by Alsa and Yggdrasil.
