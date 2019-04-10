# Cathedral Assets Optimizer


Cathedral Assets Optimizer is a tool aiming to automate asset conversion and optimization for Skyrim Special Edition. 

# Documentation

Documentation is incomplete. It is available [here](https://g_ka.gitlab.io/sse-assets-optimiser/).

# Features

General :

*    All optimization options can be performed on just a single mod or multiple mods at once.
*    You can convert, optimize, and pack your entire mod install into BSAs with a single click!
*    Each option is independently configurable
*    Dry Run mode to detect what files would be processed without performing formatting changes



BSAs :

*    Backup and extract old BSAs before processing, preserving existing loose files
*    Recreate original BSAs after processing
*    Existing loose assets can be added to the archives during repack
*    Delete backups to save space
*    Split assets into several BSAs (since a BSAs can't be greater than 2gb)


Textures :

*    Convert unsupported TGA files to DDS format
*    Uses Nifscan to detect and convert unsupported DDS formats to useable ones
*    Convert uncompressed DDS files to BC7, the highest quality compression Skyrim utilizes


Meshes :

*    Optimize both normal and headpart meshes
*    Detects headparts and applies correct settings
*    Offers three levels of mesh processing detail


Animations :

*    Uses Bethesda's Havok Tool to convert Oldrim animations to SSE format




# Requirements

* BSArch by Zilav
* The SSE Creation Kit, found in the Bethesda Launcher.
* Havok Tool provided with the Creation Kit.



# Installation

*    Download a binary from [NexusMods](https://www.nexusmods.com/skyrimspecialedition/mods/23316/) or compile this repo with QMake.
*    Download and extract the Cathedral Assets Optimizer archive into a new folder anywhere you wish.
*    Download BSArch and extract bsarch.exe into the (SSEAO)\resources folder.
*    If you intend to process animations: Download the SSE Creation Kit from the Bethesda Launcher.
*    Havok Tool should then be autodetected. If not, navigate to Skyrim Special Edition\Tools\HavokBehaviorPostProcess and copy HavokBehaviorPostProcess.exe into the (SSEAO)\resources folder.



# Use Instructions

Disclaimers :
Always make a backup of your mods.
BSA Processing is not supported when working directly with the game's Data folder.

Launch the tool outside of your mod manager.
Click Open Directory and navigate to the folder requiring conversion or optimization.
Select whether you're processing a single mod or several.
By default no options are enabled. Activate whichever you need. In "Several mods" mode only the minimum "Necessary" optimization for meshes is available.
For general use, recommended modes per mod manager are:
    Kortex, MO2, Vortex : One mod or Several mods
    NMM : One mod
    Wrye Bash : One mod (Several mods only with small mod setup or installed on SSD due to file indexing)
    Once desired options are set, click Run.
    Make a snack while you wait. SAO does automate the conversion, but speed is still determined by the number of files.



If you need help, please join us in the [Modding Tools Discord server](https://discordapp.com/invite/mraruus)!



# Credits

Zilav, for Nifscan, BSArch and ListHeadparts
Ousnius, for sharing the SSE Nif Optimizer code.
Opparco, for creating NifOpt, a CLI version of SSE Nif Optimizer.
Microsoft, for TexConv and TexDiag.
Feles Noctis, Hishy, and many others, for tests and advice