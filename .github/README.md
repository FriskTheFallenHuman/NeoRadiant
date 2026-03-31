# NeoRadiant

NeoRadiant is a fork of [DarkRadiant](https://www.darkradiant.net), a level editor for idTech4-based games. While DarkRadiant focuses primarily on The Dark Mod, NeoRadiant is tailored for editing generic maps targeting Idtech4 game engines.

## Changes from HellForge

NeoRadiant has very opinionated features, keybindings and a different workflow such as:

- Lua Scripting instead of Python
- Create and manipulate brushes directly in the 3D camera view
- Shift+scroll creates a trim brush from a face and resizes it
- Ctrl+scroll resizes hit face and opposite face symmetrically
- Dark UI by default, with a theme manager system
- Game configuration manager for managing multiple game setups
- Quickly create a sealing brush enclosing any selection
- Control the opacity of utility textures (clip, caulk) in the viewport
- Settings panel to configure anti-aliasing and other visual options across all viewports
- Command palette for quickly running and discovering operations
- Insert palette for quickly adding entties to the map
- Goto mode, similar to Sublime Text, for quick entity navigation
- Visual preview mode for browsing textures
- Thumbnail browser in the Media panel
- AAS file visualization with data exposed to Lua scripting
- Position input fields on the entity tab for precise adjustments
- Polygon tool also works in the 3D viewport
  
  

## New tools

- **Array Modifier**: Duplicates selected brushes/entities in configurable array patterns.
- **Scatter Tool**: Scatters copies of selected objects across surfaces with configurable parameters.
- **Polygon Tool**: Draw arbitrary polygon shapes in the 2D ortho views, allowing for a Doom 1-map editing workflow.
- **Decal Shooter**: Point-and-click tool for placing decal textures onto surfaces directly from the 3D camera view.
- **CSG Intersect**: Computes the intersection of selected brushes, keeping only the overlapping volume.
- **CSG Passable**: Turns solid volumes into wall shells and carves away interior geometry.
- **CSG Shell**: Similar to Passable but repositions brushes to avoid overlap.
- **CSG Bridge Tool**: Creates bridge geometry connecting two selected faces from different brushes.
- **Corner Patch Generator**: Generates corner patch meshes.
- **Sweep Tool**: Extrudes a brush profile along a curve.
- **Tile-based Map Designer**: Layout-based map design using a tile grid.
- **OSM Importer**: Experimental import of OpenStreetMap data into maps.

### Procedural generation

- **Terrain Generator**: Procedural terrain generation using Perlin and Simplex noise algorithms.
- **Stairs Generator**: Generates staircases with straight, spiral, L-shaped and U-shaped configurations.
- **Cable Generator**: Creates cable/rope geometry between points.
- **Arch Generator**: Generates arch geometry with configurable parameters.
- **Building Generator**: Generates building structures, can work with or without a reference brush.

### Engine support

- `modelscale` spawnarg handling, replacing the old scaled model export workflow
- Improved Quake 4 decl support, including guide()
- Import support for Valve VMF, Quake 1, Quake 2 and Valve 220 MAP formats

## Getting started

NeoRadiant requires game resources to work with, these resources are not installed by this editor. You'll need to point NeoRadiant to one of the supported games (Doom 3, Prey, Quake 4, etc.) before you can start working on your map.

# Compiling on Windows

## Prerequisites

NeoRadiant is built on Windows using *Microsoft Visual Studio*, the free Community Edition can be obtained here:

*VC++ 2022:* https://visualstudio.microsoft.com/downloads/ (Choose Visual Studio Community)

When installing Studio please make sure to enable the "Desktop Development with C++" workload.

For editing wxWidgets Forms Builder (.fb) you need [v3.9.0](https://github.com/wxFormBuilder/wxFormBuilder/releases/tag/v3.9.0) of wxFormBuilder, has anything above that is not supported.

## Build

The main Visual C++ solution file is located in the root folder of this repository:

`NeoRadiant.sln`

Open this file with Visual Studio and start a build by right-clicking on the top-level
"Solution 'NeoRadiant'" item and choosing Build Solution. The executable will be placed in the `install/` folder.

### Windows Build Dependencies

Since NeoRadiant requires a couple of open-source libraries that are not available on Windows by default, it will try to download and install the dependencies when the build starts. If it fails for some reason, you can try to run this script:

 `tools/scripts/download_windeps.ps1`

The dependencies packages need to be extracted into the main source directory, i.e. alongside the `include/` and `radiant/` directories.
Just drop the windeps.7z in the project folder and use 7-zip's "Extract to here"

# Compiling on Linux

## Prerequisites

To compile NeoRadiant a number of libraries (with development headers) and a standards-compliant C++17 compiler are required. On an Ubuntu system the requirements may include any or all of the following packages:

* libwxgtk3.0-dev
* libxml2-dev
* libsigc++-2.0-dev
* libftgl-dev
* libglew-dev
* libalut-dev
* libgtest-dev
* libeigen3-dev
* libgit2-dev (optional)

To generate the local offline HTML user guide, the `asciidoctor` command must be in your
PATH. This is an optional dependency: if the command is not found, the CMake build will
proceed without building the user guide.

This does not include core development tools such as g++ or the git client to download the
sources (use sudo apt-get install git for that). One possible set of packages might be:

`sudo apt-get install git cmake g++ gettext pkg-config`

## Build

To build NeoRadiant the standard CMake build process is used:
    $ cmake .
    $ make
    $ sudo make install

To install somewhere other than the default of `/usr/local`, use the `CMAKE_INSTALL_PREFIX` variable.
    $ cmake -DCMAKE_INSTALL_PREFIX=/opt/NeoRadiant
    $ make
    $ sudo make install

Other useful variables are `CMAKE_BUILD_TYPE` to choose Debug or Release builds, `ENABLE_DM_PLUGINS` to disable the building of Dark Mod specific plugins (enabled by default), and `ENABLE_RELOCATION` to control whether NeoRadiant uses hard-coded absolute paths like `/usr/lib` or paths relative to the binary (useful for certain package formats like Snappy or FlatPak).

# License

The source code is published under the [GNU General Public License 2.0 (GPLv2)](http://www.gnu.org/licenses/gpl-2.0.html
), except for a few libraries which are using the BSD or MIT licenses, see the [LICENSE](https://raw.githubusercontent.com/FriskTheFallenHuman/NeoRadiant/master/LICENSE) file for specifics.
