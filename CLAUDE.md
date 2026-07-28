# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

ForcePAD is an educational sketch-based finite element analysis tool. Users draw structural shapes with a brush, place forces and boundary conditions, and run a 2D FEM solver to see stress/displacement results. The primary build is the Qt6 port (`qtforcepad`); the original FLTK build (`forcepad`) is kept but is not the default.

## Build

The project uses CMake with vcpkg for dependency management. vcpkg is auto-detected at `E:/vcpkg`, `C:/vcpkg`, or `$VCPKG_ROOT`.

```bash
# Qt6 build (default)
cmake -B build-debug
cmake --build build-debug --config Debug

# FLTK build (legacy, opt-in)
cmake -B build-debug -DBUILD_FLTK_APP=ON
cmake --build build-debug --config Debug

# Force rebuild of a single target (e.g. after CMakeLists changes)
cmake --build build-debug --config Debug --clean-first --target qtforcepad
```

Executables land in `bin/Debug/` or `bin/Release/`. There are no tests.

### macOS: Qt6 comes from Homebrew, not vcpkg

`qtbase`/`qtsvg` are marked `"platform": "!osx"` in `vcpkg.json`, so on macOS vcpkg only supplies `libjpeg-turbo`, `libpng`, `zlib`, `opengl`, `eigen3`, and `spdlog`. Qt itself must be a prebuilt Homebrew install (`brew install qt`); the root `CMakeLists.txt` appends `/opt/homebrew/opt/qt` or `/usr/local/opt/qt` to `CMAKE_PREFIX_PATH` automatically when found, so no manual flags are needed.

This exists because vcpkg's `qtbase` port builds Qt from source on macOS, which pulls in `harfbuzz`, whose vcpkg port fails to compile against current Xcode/libc++ (`-D_LIBCPP_ENABLE_ASSERTIONS` was removed from newer libc++ headers). Do not re-add `qtbase`/`qtsvg` as unconditional vcpkg dependencies.

`macdeployqt` (found next to the Homebrew Qt6 config) already runs as a post-build step on `APPLE` in `src/qtforcepad/CMakeLists.txt`, bundling Qt frameworks/plugins into the `.app` and ad-hoc codesigning it. Its "Cannot resolve rpath" stderr lines during that step are benign — they come from macdeployqt's own dependency scan and don't affect the linked binary (verify with `otool -L` on the built binary if in doubt).

`PaintView::loadBrushes()`/`setColorMap()` (`src/paintview/PaintView.cpp`) resolve the `brushes`/`colormaps` directories relative to `argv[0]`'s directory. On macOS that's `qtforcepad.app/Contents/MacOS/`, one level below the committed `bin/release/{brushes,colormaps}` assets, so `src/qtforcepad/CMakeLists.txt` has an `APPLE`-only `POST_BUILD` step (next to the existing SVG-icons copy) that copies both directories into `Contents/MacOS/` — same trick as the icons copy, just for a different pair of asset dirs.

### macOS: App Sandbox entitlements (Mac App Store prep)

`src/qtforcepad/qtforcepad.entitlements` declares `com.apple.security.app-sandbox`, `com.apple.security.network.client` (for `ModelUrlLoader`'s http(s) model downloads), and `com.apple.security.files.user-selected.read-write` (for `QFileDialog`-based open/save of `.fp2` files). The `codesign` post-build step in `src/qtforcepad/CMakeLists.txt` passes `--entitlements` pointing at it, alongside the existing ad-hoc `--sign -`.

Ad-hoc signing is sufficient to have the sandbox *enforced* locally for the container and network-client entitlements (no paid Developer ID needed) — confirmed by the app creating a real container at `~/Library/Containers/se.lth.byggmek.forcepad/` on launch, and by launching with an http(s) URL positional argument (e.g. `qtforcepad https://example.com/test.fp2`) and seeing an actual HTTP response in the log rather than a sandbox connection-denied error.

**Ad-hoc signing is *not* sufficient for `files.user-selected.read-write`, though.** Confirmed via `log stream --predicate '... OR subsystem == "com.apple.sandbox"'` while manually using File > Open / Save through the real native panels: `com.apple.scopedbookmarksagent.xpc` (the Powerbox security-scoped-bookmark agent) does get invoked — `CFURLCreateBookmarkData`/`CFURLResolveBookmarkData` calls show up around the dialog interaction — but the kernel sandbox check still denies the actual access afterward, e.g. `Sandbox: qtforcepad(94360) deny(1) file-write-create /Users/lindemann/Desktop/test.fp2` for a file picked through the real Save panel, and `deny(1) file-read-data ... beam_sym.fp2` for one picked through Open. The Powerbox grant is validated against the requesting process's code signing identity, and an ad-hoc signature (`--sign -`, no Team ID, a different CDHash every rebuild) doesn't carry a stable enough identity for that validation to succeed. **A real "Apple Development" signing identity is needed to properly test file-open/save entitlements locally** — this can be a free one (Xcode → Settings → Accounts → add an Apple ID → Manage Certificates → create "Apple Development"), distinct from the paid Apple Developer Program membership needed for actual Mac App Store submission. Don't conclude the entitlements file is wrong just from an ad-hoc-signed test showing this deny — the entitlement itself is the standard, correct one for `NSOpenPanel`/`NSSavePanel`; the *signing identity* is what's insufficient here.

For actual Mac App Store submission (not just local sandbox testing), this entitlements file is necessary but not sufficient — still needed: a Developer ID/Mac App Distribution signing identity (replacing ad-hoc `--sign -`), a provisioning profile, and packaging as a signed `.pkg` via `productbuild`. See conversation history / ask before assuming any of that is wired up.

**Sandbox exposed a latent crash in `PaintView::openModel(filename)`** (`src/paintview/PaintView.cpp`): it opened the model file with `fstream::open()` and passed the stream straight to `FemGrid2::readFromStream()` without checking `is_open()`. `readFromStream()` (`src/common/FemGrid2.cpp`) reads `width`/`height` into uninitialized `int` locals via `in >> width >> height` — if the stream never successfully opened, `operator>>`'s sentry sees the already-failed stream and returns without touching the targets at all (different from a stream that opens fine but fails mid-parse, which zeroes them per the C++11 standard), so `width`/`height` stay as garbage stack values, and the resulting `image->setSize(width, height)` throws `std::bad_alloc`. Under App Sandbox, any file path not granted via the Powerbox (i.e. not chosen through the native Open/Save panel — a raw/hardcoded absolute path, for instance) fails to open silently, so this was trivial to trigger once sandboxing was added, though the code path was already latently unsafe before that. Now fixed with an `is_open()` check that logs via `fp_error` and bails out gracefully. If you see a similar crash reappear for a *dialog-picked* file specifically (not just a raw path), that would point to something deeper in the Powerbox/entitlement interaction and needs separate investigation — this fix only turns the crash into a clean log message, it doesn't by itself guarantee the native panel grants access in every scenario.

**macOS gotcha — don't let `find_package(OpenGL)` pick up Homebrew's Mesa GL**: CMake auto-adds `/opt/homebrew` to its default search prefixes on Apple Silicon. If `mesa`/`mesa-glu`/`glew` happen to be installed there (a common transitive pull-in from unrelated Homebrew formulas, not something this project asks for), `find_package(OpenGL)` can resolve `OPENGL_gl_LIBRARY` to Homebrew's `libGL.dylib` instead of the system `OpenGL.framework` that Qt's own frameworks link against. Mixing the two GL implementations makes the linked app depend on `libgallium-*.dylib`/X11 (`libxcb*`) libraries that never get bundled, so it crashes at launch with a `dyld` "Library not loaded" error — Release/Debug optimization has nothing to do with it. The root `CMakeLists.txt` forces `OPENGL_gl_LIBRARY`/`OPENGL_INCLUDE_DIR` to the system framework (resolved via `xcrun --sdk macosx --show-sdk-path`, since `/System/Library/Frameworks/OpenGL.framework` has no `Headers` on-disk on modern macOS — only the SDK copy does) unconditionally on `APPLE`. **This must run before `find_package(Qt6 ...)`**, not merely before the project's own later `find_package(OpenGL REQUIRED)` — `Qt6GuiConfig.cmake` triggers its own internal `find_dependency(WrapOpenGL)` → `find_package(OpenGL)` as a side effect of `find_package(Qt6)`, and once the `OpenGL::GL` imported target exists, its `IMPORTED_LOCATION` is locked in; a cache override afterward has no effect. If you ever see this crash again, check `otool -L` on the built binary and on `Contents/Frameworks/libGL.1.dylib` (if present) before assuming it's something else — and note `macdeployqt -always-overwrite` never *removes* stale files from a previously-broken bundle, so `rm -rf bin/release/qtforcepad.app` before rebuilding when diagnosing bundle-content issues.

### Qt runtime deployment (Windows)

The post-build step in `src/qtforcepad/CMakeLists.txt` copies Qt plugin directories (`platforms/`, `styles/`) and `Qt6Svg(d).dll` next to the executable. If plugins are missing after a build, it usually means the target was already up-to-date and MSBuild skipped it — use `--clean-first` to force the post-build step to re-run.

## Architecture

### Two parallel UI targets

| Target | Directory | UI framework | Preprocessor |
|---|---|---|---|
| `forcepad` | `src/forcepad/` | FLTK + OpenGL | *(none)* |
| `qtforcepad` | `src/qtforcepad/` | Qt6 + QOpenGLWidget | `USE_QT`, `QT_NO_KEYWORDS` |

Both targets share the same static libraries: `common`, `ivf2d`, `calfem`, `fplog`.

### The `fp::PaintView` bridge pattern

`fp::PaintView` (`src/paintview/PaintView.h/cpp`) is the core logic class — ~3600 lines of framework-agnostic C++. It owns the FEM grid, brush/drawing state, OpenGL rendering, and event dispatch. It declares virtual methods for all UI interactions:

```cpp
virtual int height(); virtual int width();
virtual void doRedraw(); virtual void doMakeCurrent();
virtual const std::string doSaveDialog(...);
virtual bool doNewModel(int &w, int &h, int &initialStiffness);
// etc.
```

Each UI port provides a concrete subclass that inherits from both a framework widget and `fp::PaintView`:

- **FLTK**: `FlPaintView : public Fl_Gl_Window, public fp::PaintView` (`src/forcepad/FlPaintView.h/cpp`)
- **Qt6**: `QtPaintView : public QOpenGLWidget, public fp::PaintView` (`src/qtforcepad/QtPaintView.h/cpp`)

When adding UI-specific behaviour to `PaintView.cpp`, guard it with `#ifndef USE_QT` / `#else`.

### Shared library: `common`

Contains the FEM data model and purely algorithmic code: `FemGrid2`, `Node`, `Element`, `Force`, `Constraint`, `ColorMap`, `ImageGrid2`, `ForcePadClipboard`, etc.

**Important**: `FemGridSolver2.cpp` is excluded from the `common` library glob and compiled directly into each executable target. This is because it has framework-specific dependencies (`Fl::check()`/`QCoreApplication::processEvents()`) that differ between the FLTK and Qt builds. Do not add it back to the glob.

`LogWindow.cpp` and other `Fl_*` files in `src/common/` are FLTK-specific and get pulled into `commond.lib`. They are harmless for the Qt build only so long as `FemGridSolver2.cpp` is excluded from `common` (otherwise `FemGridSolver2.obj` would reference `LogWindow` and pull in FLTK symbols at Qt link time).

### FEM solver

`fp::FemGridSolver2` (`src/common/FemGridSolver2.h/cpp`) assembles and solves the 2D plane-stress FEM problem using `calfem` (CALFEM C++ port) and Eigen for matrix algebra. It also runs topology optimisation (Sigmund filter). It is compiled separately into each executable because of the `Fl::check()` / `QCoreApplication::processEvents()` difference.

### FLTK dialogs

All FLTK dialogs in `src/forcepad/` are generated by FLUID (Fast Light User Interface Designer) from `.fl` source files. **Edit the `.fl` file, not the generated `.h/.cpp`**. The generated pair has the same base name.

### Logging: `fplog` library

`src/fplog/` is a thin spdlog wrapper that works in both the FLTK and Qt builds.

- **`FPLog.h`** exposes `fp_debug/fp_info/fp_warn/fp_error(ctx, fmt, ...)` macros (fmt-style) and a `FPLog::init()` function that wires up sinks.
- **`FPLog.cpp`** sets up a console sink by default; callers can pass additional sinks (e.g. a Qt widget sink) to `FPLog::init()`.
- **`so_print` compatibility**: `FPLog.h` defines `so_print(ctx, msg)` as `spdlog::info(...)` so legacy calls compile in both builds, but only when `LogWindow.h` hasn't already defined it. Include `FPLog.h` instead of `LogWindow.h` in any file shared between FLTK and Qt.

**Qt log sink** (`src/qtforcepad/QtLogSink.h`): a `spdlog::sinks::base_sink` subclass that appends colour-coded messages to a `QPlainTextEdit` via `QMetaObject::invokeMethod(..., Qt::QueuedConnection)`, making it safe to call from the solver thread. It is instantiated in `MainWindow` and passed to `FPLog::init()` at startup.

**Old pattern** (still present in `src/common/LogWindow.h` for the FLTK build): `so_print(context, message)` routes to an FLTK log window. Do not include `LogWindow.h` in files compiled into the Qt build without a `#ifndef USE_QT` guard.

### FEM solver logging

`FemGridSolver2.cpp` now includes `FPLog.h` and uses `fp_debug/fp_info/fp_warn` throughout instead of bare `so_print`. It also has a local `#define NOMINMAX` before `<windows.h>` on WIN32 to guard against the `min`/`max` macro conflict independently of the global CMake definition.

### OpenMP and Eigen

OpenMP is enabled via `find_package(OpenMP COMPONENTS CXX)` in the root `CMakeLists.txt` (optional — build succeeds without it). `OpenMP::OpenMP_CXX` is linked into `calfem`, `common`, and `qtforcepad`, each gated on the aggregate `OpenMP_FOUND`. Eigen detects the `-fopenmp` / `/openmp` flag via `_OPENMP` and parallelizes dense matrix products and some decompositions automatically — no source changes needed.

**Windows/MSVC gotcha**: `<windows.h>` defines `min` and `max` as macros by default, which breaks `std::min`/`std::max` calls inside Eigen's `Parallelizer.h` when OpenMP is active. The root `CMakeLists.txt` defines `NOMINMAX` globally on Windows to prevent this. Do not remove it.

**macOS gotcha**: AppleClang ships no OpenMP runtime, so plain `find_package(OpenMP)` always fails on macOS even with `libomp` installed — it needs the separate frontend flag/library pointed at explicitly. The root `CMakeLists.txt` retries with hints toward Homebrew's `libomp` (`/opt/homebrew/opt/libomp` or `/usr/local/opt/libomp`) when the plain lookup fails; if that directory doesn't exist (`brew install libomp` not run), it's a no-op and the solver/Eigen fall back to single-threaded, which is a common reason a macOS build feels slower than the Windows/Linux ones for the same model. Note the `COMPONENTS CXX` on the `find_package(OpenMP ...)` call is load-bearing: the project enables both C and CXX languages by default, so an unqualified `find_package(OpenMP)` requires the *C* frontend's OpenMP support too — the Homebrew-libomp hint above only patches the CXX-side variables, so without `COMPONENTS CXX` the aggregate `OpenMP_FOUND` silently stays false (even once `libomp` is installed and `OpenMP_CXX_FOUND` is true) and nothing ends up linking OpenMP at all, since every consumer gates on the aggregate. Verify with `otool -L` on the built binary for `libomp.dylib` if you ever need to confirm it's actually engaged, rather than trusting the CMake status lines alone.
