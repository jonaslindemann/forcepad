# ForcePAD

[![Build](https://github.com/jonaslindemann/forcepad/actions/workflows/forcepad.yml/badge.svg)](https://github.com/jonaslindemann/forcepad/actions/workflows/forcepad.yml)
[![License: GPL v2](https://img.shields.io/badge/License-GPL_v2-blue.svg)](COPYING)

Sketch structures and instantly explore forces, stresses, deformations, and topology optimisation.

ForcePAD is an open-source, sketch-based 2D finite element analysis tool for structural mechanics education and experimentation. Draw a structural shape like in a paint program, add loads and supports, and inspect the finite element response interactively.

[Download](https://github.com/jonaslindemann/forcepad/releases/latest) |
[Documentation](https://jonaslindemann.github.io/forcepad/) |
[Quick start](https://jonaslindemann.github.io/forcepad/quick-start/) |
[Examples](https://jonaslindemann.github.io/forcepad/examples/) |
[Videos](https://jonaslindemann.github.io/forcepad/videos/)

![ForcePAD main window](docs/docs/images/screenshot.png)

> Media placeholder: add a 5-10 second animated GIF here showing the complete workflow: sketch a beam, add supports, add a force, switch to Action mode, and move the force while the stresses update.

## Why ForcePAD?

- Sketch-based modelling: create structures with brushes, fill tools, lines, rectangles, and ellipses.
- Immediate feedback: view displacements, principal stresses, and von Mises stresses without a heavyweight CAD workflow.
- Interactive exploration: move and rotate forces while the structural response updates.
- Topology optimisation: run optimisation experiments from an initial design domain.
- Educational focus: designed for teaching finite element analysis, structural mechanics, stress flow, and deformation behaviour.
- Open source: GPL-2 licensed C++ code using Qt, OpenGL, Eigen, and a C++ CALFEM implementation.

## Quick Start

1. [Download the latest release](https://github.com/jonaslindemann/forcepad/releases/latest).
2. Open ForcePAD and create a new model, or load one of the bundled `.fp2` examples.
3. Use Sketch mode to draw material on the canvas.
4. Use Physics mode to add constraints and forces.
5. Use Action mode to inspect stresses, displacements, and topology optimisation results.

See the full [Quick start in 60 seconds](https://jonaslindemann.github.io/forcepad/quick-start/) guide for screenshots.

## Example Projects

The repository includes sample `.fp2` models under `bin/release/samples/`, including beam, block, thick beam, and Pantheon-inspired examples. See the [Examples](https://jonaslindemann.github.io/forcepad/examples/) page for a guided list.

## Feature Comparison

| Feature | ForcePAD | Typical commercial FEA workflow |
| --- | --- | --- |
| Open source | Yes | Usually no |
| Sketch-based modelling | Yes | Usually no |
| Real-time interaction | Yes | Often limited |
| Educational finite element focus | Strong | Varies |
| Lightweight desktop workflow | Yes | Often heavier |
| Topology optimisation experiments | Yes | Varies by product/license |

## Build From Source

ForcePAD uses CMake and vcpkg. The primary maintained target is `qtforcepad`.

```powershell
cmake -B build-debug
cmake --build build-debug --config Debug
```

The executable is written to `bin/Debug/qtforcepad.exe` on Windows. More detailed build notes are available in the [developer documentation](https://jonaslindemann.github.io/forcepad/develop/).

## Academic Use

ForcePAD was developed at the Division of Structural Mechanics, Lund University, and has been used in structural mechanics education and research. If ForcePAD helps your teaching, research, or publications, see [Cite ForcePAD](https://jonaslindemann.github.io/forcepad/cite/) or the repository [CITATION.cff](CITATION.cff).

## Community

- Report bugs and request features in [GitHub Issues](https://github.com/jonaslindemann/forcepad/issues).
- Read [CONTRIBUTING.md](CONTRIBUTING.md) before opening a pull request.
- Use the [Roadmap](ROADMAP.md) to see planned project directions.
- GitHub Discussions is a good fit for teaching questions, example requests, and general usage support once enabled in the repository settings.
