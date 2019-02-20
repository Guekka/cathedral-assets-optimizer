# SSE Assets Optimizer

## Description

SSE Assets Optimizer is a tool aiming to automate assets optimization for Skyrim Special Edition.

**Meshes :**

 * It optimizes hard crashing meshes, headparts and lightly optimize other meshes. 
 * New method to handle headparts

**Textures :**

 * It converts uncompressed normal maps to BC7 
 * It converts TGA to DDS. 
 * It uses Nifscan to convert incompatible DDS


**Animations :**
 * It uses Bethesda's Havok Tool to port Oldrim animations.


**BSA :**
 * It extracts the old BSA before processing (existing loose files are preserved)
 * Rename it 
 * Create a new one


All of this can be done on **multiple mods at once**, meaning that you can pack your whole load order in BSA after optimizing it, with only one click. 
Each option is **independently configurable.**




## Compilation

Compilation was only tested with Qt Creator and GCC. 
Simply clone this repo and import the pro file in Qt Creator.