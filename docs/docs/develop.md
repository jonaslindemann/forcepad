# Build and Develop ForcePAD

ForcePAD is a C++ application built with CMake and managed dependencies via vcpkg. The primary UI target is `qtforcepad` (Qt6).

## Prerequisites

1. **CMake** 3.24 or higher — [cmake.org](https://cmake.org/download/)
2. **C++ compiler** with C++17 support
   - Windows: Visual Studio 2022 (with Desktop C++ workload)
3. **Git** — [git-scm.com](https://git-scm.com/)
4. **vcpkg** — see below

## vcpkg setup

ForcePAD uses [vcpkg](https://github.com/Microsoft/vcpkg) for dependency management. CMake auto-detects vcpkg in the following locations:

- `VCPKG_ROOT` environment variable
- `E:/vcpkg`
- `C:/vcpkg`

If vcpkg is not installed:

```powershell
git clone https://github.com/Microsoft/vcpkg.git C:/vcpkg
C:/vcpkg/bootstrap-vcpkg.bat
$env:VCPKG_ROOT = "C:/vcpkg"
```

## Building

### Default build (Qt6)

```powershell
cmake -B build-debug
cmake --build build-debug --config Debug
```

The executable is placed in `bin/Debug/qtforcepad.exe`.

### Force-rebuild a single target

If you change `CMakeLists.txt` or need the post-build steps to re-run (e.g., Qt plugin deployment):

```powershell
cmake --build build-debug --config Debug --clean-first --target qtforcepad
```

### Release build

```powershell
cmake -B build-release
cmake --build build-release --config Release
```

Executables land in `bin/Release/`.

## Qt runtime deployment (Windows)

The post-build step in `src/qtforcepad/CMakeLists.txt` automatically copies the required Qt plugin directories (`platforms/`, `styles/`) and `Qt6Svgd.dll` next to the executable. If Qt plugins are missing after a build, the target was likely already up-to-date and MSBuild skipped the post-build step — use `--clean-first` to force it to re-run.

## Architecture overview

### Core libraries (shared)

| Library | Location | Description |
| --- | --- | --- |
| `common` | `src/common/` | FEM data model: `FemGrid2`, `Node`, `Element`, `Force`, `Constraint`, `ColorMap`, etc. |
| `ivf2d` | `src/ivf2d/` | 2D OpenGL rendering primitives |
| `calfem` | `src/calfem/` | CALFEM C++ port — FEM routines |
| `fplog` | `src/fplog/` | Logging wrapper (spdlog) |

### The `fp::PaintView` bridge pattern

`fp::PaintView` (`src/paintview/PaintView.h/cpp`) is the ~3600-line core logic class. It owns the FEM grid, brush/drawing state, OpenGL rendering, and event dispatch, and declares virtual methods for all UI interactions:

```cpp
virtual int height(); virtual int width();
virtual void doRedraw(); virtual void doMakeCurrent();
virtual const std::string doSaveDialog(...);
virtual bool doNewModel(int &w, int &h, int &initialStiffness);
```

The Qt6 port provides a concrete subclass that inherits from both the framework widget and `fp::PaintView`:

- **Qt6**: `QtPaintView : public QOpenGLWidget, public fp::PaintView` (`src/qtforcepad/QtPaintView.h`)

### FEM solver

`fp::FemGridSolver2` (`src/common/FemGridSolver2.h/cpp`) assembles and solves the 2D plane-stress FEM problem and runs topology optimisation. It is **compiled directly into each executable** (not into the `common` library) because it calls `QCoreApplication::processEvents()` to keep the UI responsive during long solves. Do not add it back to the `common` source glob.

### Logging

All logging uses the `FPLog` wrapper (`src/fplog/FPLog.h`). Use the fmt-style macros:

```cpp
#include "FPLog.h"

fp_debug("MyClass", "entering function");
fp_info("MyClass", "loaded {} elements", count);
fp_warn("MyClass", "no boundary conditions found");
fp_error("MyClass", "solver failed: {}", reason);
```

`FPLog::init()` wires up sinks at startup. The Qt build adds a `QtLogSink` (`src/qtforcepad/QtLogSink.h`) that appends colour-coded messages to the log panel in the main window. Do not include `LogWindow.h` in any file compiled into the Qt build.

## Contributing

1. Fork the repository on GitHub.
2. Create a feature branch.
3. Verify the change builds for the `qtforcepad` target.
4. Submit a pull request.
