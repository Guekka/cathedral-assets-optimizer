---
description: Product purpose and UX constraints for CAO contributor changes.
applyTo: 'src/gui/**,docs/**'
---

# CAO product purpose and UX context

This is contributor context, not user documentation. Treat it as a working product model, and recheck
the code and current guides when supported games or workflows change.

## Raison d'etre

CAO automates specialized, repeatable batch processing of Bethesda mod assets for a target game. It
converts or optimizes supported meshes, textures, and animations, and can extract or create
game-specific BSA archives. A prominent workflow is preparing Skyrim LE assets for Skyrim SE, but
porting is not the only use case.

CAO is often one step in a larger modding workflow, not an end-to-end modlist installer or plugin
conversion tool. A guide may use it for only one task, such as creating BSAs, while another tool
handles textures or meshes. Plugins that require resaving may need separate Creation Kit work, and
processed mods should be tested in game.

## How people use it

- **Quick Optimize** is a simplified, one-click view for common tasks. Preserve this direct path when
  redesigning settings or navigation.
- Some users arrive from a modlist guide with a specific working folder, target game, profile, and
  exact operation to perform. The Phoenix Flavour guide, for example, uses CAO primarily for BSA
  creation and recommends preserving source files; Salvo gives users a prescribed folder and settings.
- Other users process large collections and need repeatable profiles, clear logs, and a way to see
  which files or mods actually need work.
- Users may intentionally use CAO for one asset class or operation, then use other tools for the
  rest. Do not assume every user wants to run every available module.

## UX implications

- Begin from the user's task and selected input, then make the target game/profile and operation
  explicit. A task-specific profile can be more meaningful than exposing every setting immediately.
- Make the processing scope clear: one mod versus several immediate subfolders, and which files are
  matched, processed, skipped, or left unchanged.
- Make side effects visible before running. CAO can modify or remove files; a dry run is not a
  backup. Clearly identify options such as deleting loose source files after BSA creation.
- Keep routing previews distinct from processing dry runs. A routing preview explains which settings
  apply; a dry run reports planned processing steps.
- Treat file rules as an advanced customization mechanism, not the product's purpose or the default
  mental model.
- Avoid promising that conversion is universally safe, fixes every mod, or improves performance in
  every case. Explain limitations and encourage users to preserve and test their working copies.
- Preserve guide-driven workflows: don't rename or regroup operations in ways that make established
  instructions unreliable without updating those guides.

## Evidence and caveats

- The [README](../../README.md), [user guide](../../docs/user-guide.md), [porting guide](../../docs/porting-skyrim-le-to-sse.md),
  [asset processing code](../../src/main_process.cpp), and [BSA processing code](../../src/bsa_process.cpp)
  describe the current project behavior.
- The [Phoenix Flavour CAO setup](https://github.com/foreverphoenix/the-phoenix-flavour/blob/d3f384e677683c114513c47b6a7d9e9a1fb5207d/content/bg/tool-setup/cao.md)
  and [Salvo workflow](https://salamand3r.fail/salvo/running-the-things) are examples of guide-driven use,
  not universal recommendations for every CAO user.
- GitHub feedback includes reports about mode and pattern confusion ([#5](https://github.com/Guekka/cathedral-assets-optimizer/issues/5),
  [#15](https://github.com/Guekka/cathedral-assets-optimizer/issues/15)), scripted batch use ([#23](https://github.com/Guekka/cathedral-assets-optimizer/issues/23)),
  and large-batch logging and backup needs ([#25](https://github.com/Guekka/cathedral-assets-optimizer/issues/25)).
  These are useful signals, not a representative survey.
- Public feedback spans different CAO versions. Distinguish the experimental CAO7 interface from
  older stable releases before applying feedback or instructions to current behavior.
