# SSE Assets Optimiser

This tool aims to automate assets optimisation for Skyrim Special Edition. 
Made with Qt and C++. Command line arguments currently not supported. 

Meshes : 
It uses Nifscan to detect hard crashing meshes, and only optimize them. 
This method can have downsides. I plan to create a tweaked Nif Optimizer.

Textures :
It uses Texconv to automatically convert uncompressed normal maps to BC7 and to convert TGA to DDS. It also use Nifscan to convert incompatible DDS. 

Animations : 
It uses Havok Tool to port Oldrim animations. 

BSA : 
It uses BSArch to extract the old BSA, and to create a new one. 