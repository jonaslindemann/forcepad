# Downloading ForcePAD

[Latest release](https://github.com/jonaslindemann/forcepad/releases/latest){ .md-button .md-button--primary }

All releases of ForcePAD can be downloaded from GitHub:

[https://github.com/jonaslindemann/forcepad/releases](https://github.com/jonaslindemann/forcepad/releases)

## Installation

### Windows

Download the installer (`.exe`) from the releases page and run it. After installation, ForcePAD can be started from the Start menu under **Structural Mechanics → ForcePAD**.

### Building from source

If you want the latest development version or need to build for a platform without a pre-built binary, see the [Develop](develop.md) page for full build instructions using CMake and vcpkg.

## First Run

After installing ForcePAD, follow the [Quick start](quick-start.md) guide:

1. Create or open a model.
2. Draw a structure in Sketch mode.
3. Add constraints and forces in Physics mode.
4. Inspect stresses and displacements in Action mode.

Sample `.fp2` models are listed on the [Examples](examples.md) page.

## Bug reports and feature requests

Please report issues using the GitHub issue tracker:

[https://github.com/jonaslindemann/forcepad/issues](https://github.com/jonaslindemann/forcepad/issues)

## Known issues

- The ForcePAD window must be manually resized after the drawing surface is changed; use the **expand workspace** button in sketch mode to fit the surface to the new window size.
- HiDPI / high-resolution display scaling may require adjustments on some platforms.
