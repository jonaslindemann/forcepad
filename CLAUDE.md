# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

ForcePAD is an educational sketch-based finite element analysis tool. Users draw structural shapes with a brush, place forces and boundary conditions, and run a 2D FEM solver to see stress/displacement results. The only build is the Qt6 port (`qtforcepad`).

**The legacy FLTK port was retired on 2026-08-01.** The `BUILD_FLTK_APP` option, the `src/forcepad/`, `src/forcepad_r/` and `src/forcepad_kiosk/` trees, all `Fl_*`/`fl_*` sources in `src/common/`, the v1 `FemGrid`/`ImageGrid` classes and `src/common/obsolete/` are gone. Recover from git history if ever needed. Some `#ifndef USE_QT` branches still remain inside `src/paintview/PaintView.cpp` and are scheduled for a dedicated collapse pass; one of them still `#include`s the deleted `MainFrame2.h`, which is harmless because `USE_QT` is always defined now.

## Build

The project uses CMake with vcpkg for dependency management. vcpkg is auto-detected at `E:/vcpkg`, `C:/vcpkg`, or `$VCPKG_ROOT`.

```bash
# Qt6 build (the only build)
cmake -B build-debug
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

For actual Mac App Store submission (not just local sandbox testing), this entitlements file is necessary but not sufficient — still needed: an Apple Distribution signing identity (replacing ad-hoc `--sign -`), a provisioning profile, and packaging as a signed `.pkg` via `productbuild`. The signing-identity and `.pkg` parts are now wired up via `FORCEPAD_MACOS_DISTRIBUTION=appstore` — see the section below. The provisioning profile still is not (no such file is committed).

**Sandbox exposed a latent crash in `PaintView::openModel(filename)`** (`src/paintview/PaintView.cpp`): it opened the model file with `fstream::open()` and passed the stream straight to `FemGrid2::readFromStream()` without checking `is_open()`. `readFromStream()` (`src/common/FemGrid2.cpp`) reads `width`/`height` into uninitialized `int` locals via `in >> width >> height` — if the stream never successfully opened, `operator>>`'s sentry sees the already-failed stream and returns without touching the targets at all (different from a stream that opens fine but fails mid-parse, which zeroes them per the C++11 standard), so `width`/`height` stay as garbage stack values, and the resulting `image->setSize(width, height)` throws `std::bad_alloc`. Under App Sandbox, any file path not granted via the Powerbox (i.e. not chosen through the native Open/Save panel — a raw/hardcoded absolute path, for instance) fails to open silently, so this was trivial to trigger once sandboxing was added, though the code path was already latently unsafe before that. Now fixed with an `is_open()` check that logs via `fp_error` and bails out gracefully. If you see a similar crash reappear for a *dialog-picked* file specifically (not just a raw path), that would point to something deeper in the Powerbox/entitlement interaction and needs separate investigation — this fix only turns the crash into a clean log message, it doesn't by itself guarantee the native panel grants access in every scenario.

**macOS gotcha — don't let `find_package(OpenGL)` pick up Homebrew's Mesa GL**: CMake auto-adds `/opt/homebrew` to its default search prefixes on Apple Silicon. If `mesa`/`mesa-glu`/`glew` happen to be installed there (a common transitive pull-in from unrelated Homebrew formulas, not something this project asks for), `find_package(OpenGL)` can resolve `OPENGL_gl_LIBRARY` to Homebrew's `libGL.dylib` instead of the system `OpenGL.framework` that Qt's own frameworks link against. Mixing the two GL implementations makes the linked app depend on `libgallium-*.dylib`/X11 (`libxcb*`) libraries that never get bundled, so it crashes at launch with a `dyld` "Library not loaded" error — Release/Debug optimization has nothing to do with it. The root `CMakeLists.txt` forces `OPENGL_gl_LIBRARY`/`OPENGL_INCLUDE_DIR` to the system framework (resolved via `xcrun --sdk macosx --show-sdk-path`, since `/System/Library/Frameworks/OpenGL.framework` has no `Headers` on-disk on modern macOS — only the SDK copy does) unconditionally on `APPLE`. **This must run before `find_package(Qt6 ...)`**, not merely before the project's own later `find_package(OpenGL REQUIRED)` — `Qt6GuiConfig.cmake` triggers its own internal `find_dependency(WrapOpenGL)` → `find_package(OpenGL)` as a side effect of `find_package(Qt6)`, and once the `OpenGL::GL` imported target exists, its `IMPORTED_LOCATION` is locked in; a cache override afterward has no effect. If you ever see this crash again, check `otool -L` on the built binary and on `Contents/Frameworks/libGL.1.dylib` (if present) before assuming it's something else — and note `macdeployqt -always-overwrite` never *removes* stale files from a previously-broken bundle, so `rm -rf bin/release/qtforcepad.app` before rebuilding when diagnosing bundle-content issues.

### macOS: signed distribution packaging (`FORCEPAD_MACOS_DISTRIBUTION`)

`src/qtforcepad/CMakeLists.txt` has a cache option `FORCEPAD_MACOS_DISTRIBUTION` (`dev` | `standalone` | `appstore`, default `dev`) that selects the whole signing/entitlements/packaging path in one switch. Set it with `cmake -B build-release -DFORCEPAD_MACOS_DISTRIBUTION=standalone`. It does not require a Debug/Release-specific build tree — pick whichever `CMAKE_BUILD_TYPE`/config you'd normally use.

- **`dev`** (default, unchanged from before this option existed): ad-hoc, or a local "Apple Development" identity if one's installed. Sandboxed entitlements (`qtforcepad.entitlements`). This is what the App Sandbox/Powerbox section above describes and is for local testing only.
- **`standalone`**: signs with a "Developer ID Application" identity, enables Hardened Runtime (`codesign --options runtime`), and drops App Sandbox entirely — sandboxing is optional outside the Mac App Store, and dropping it sidesteps the `files.user-selected.read-write`/Powerbox caveats above (no entitlements file is used at all; hardened runtime needs one only for specific exceptions like JIT, which this app doesn't need). Two extra build targets build on top of the signed `.app`:
  - `qtforcepad_dmg` — stages the bundle plus an `/Applications` symlink and runs `hdiutil` to produce `bin/<debug|release>/ForcePAD-<version>.dmg`, matching whichever `CMAKE_BUILD_TYPE`/config the build tree was configured with (`$<LOWER_CASE:$<CONFIG>>`, so it works for both single- and multi-config generators). `qtforcepad_pkg` (appstore channel, below) uses the same convention.
  - `qtforcepad_notarize` — depends on `qtforcepad_dmg`, then runs `xcrun notarytool submit --wait` followed by `xcrun stapler staple` on the DMG. Requires the cache var `FORCEPAD_NOTARY_PROFILE` to name a keychain profile created once via `xcrun notarytool store-credentials <profile> --apple-id <id> --team-id <TEAMID> --password <app-specific-password>` (an app-specific password from appleid.apple.com, not the Apple ID password itself). Without it configure only warns, it doesn't fail — but the target will fail when actually run.

  These are separate targets rather than `qtforcepad` POST_BUILD steps because notarization can take anywhere from under a minute to tens of minutes and shouldn't run on every incremental build: `cmake --build build-release --target qtforcepad_notarize`.
- **`appstore`**: signs with an "Apple Distribution" identity, falling back to the older "3rd Party Mac Developer Application" name for the same cert type (some existing Developer accounts still have that one instead — confirmed on this machine's keychain). Keeps the sandboxed entitlements (same file as `dev`). Adds:
  - Optional provisioning-profile embedding: if a file exists at the path in cache var `FORCEPAD_PROVISION_PROFILE` (defaults to `src/qtforcepad/embedded.provisionprofile`, not committed), it's copied to `Contents/embedded.provisionprofile` as a POST_BUILD step. **No such file exists in this repo** — App Store Connect will reject a submission without one; configure only warns about this, it doesn't fail, since the `.app` still builds and signs fine locally either way.
  - `qtforcepad_pkg` target — runs `productbuild --component <bundle> /Applications --sign <installer-identity> ForcePAD-<version>.pkg`, using a "Mac Installer Distribution"/"3rd Party Mac Developer Installer" identity auto-detected the same way. Warns at configure time if no such identity is found.

All three channels auto-detect their signing identity from `security find-identity -v -p codesigning` (same pattern the pre-existing `dev`-channel "Apple Development" detection used) and fall back to ad-hoc (`-`) with a `message(WARNING ...)` if the right cert isn't installed, rather than failing configure — so switching channels never breaks a build, it just produces output that can't be notarized/submitted until the right certificate exists in the keychain. **On the machine this was built on, only "Apple Development" and "3rd Party Mac Developer Application" identities are present — there is no "Developer ID Application" certificate yet**, so `standalone` builds ad-hoc-signed output until one is created (Apple Developer portal → Certificates → + → "Developer ID Application"; requires the paid Developer Program membership, distinct from the free "Apple Development" identity already in use). Don't assume `standalone` is producing genuinely distributable output without checking `qtforcepad codesign identity:` in the configure log first.

The `FORCEPAD_VERSION` CMake variable (currently `"2.7"`, set once near the top of the `APPLE` block in `src/qtforcepad/CMakeLists.txt`) now drives both `MACOSX_BUNDLE_*_VERSION_STRING` and the `.dmg`/`.pkg` output filenames — bump it there, not in two places.

### Qt runtime deployment (Windows)

The post-build step in `src/qtforcepad/CMakeLists.txt` copies Qt plugin directories (`platforms/`, `styles/`) and `Qt6Svg(d).dll` next to the executable. If plugins are missing after a build, it usually means the target was already up-to-date and MSBuild skipped it — use `--clean-first` to force the post-build step to re-run.

## UML class diagrams (clang-uml + PlantUML)

`scripts\gen-uml.cmd` generates the class diagrams defined in the root `.clang-uml` into `docs/uml/` and renders them to SVG. `--list` shows the diagram names, `--diagram <name>` generates one, `--no-render` stops after the `.puml` sources, `--fresh` reconfigures, `--fetch-plantuml` downloads `plantuml.jar` into `tools/`.

The non-obvious part is the compilation database. clang-uml parses the real sources with libclang and therefore needs a `compile_commands.json`, but the default Visual Studio generator cannot emit one — `CMAKE_EXPORT_COMPILE_COMMANDS` is a no-op there. The script therefore configures a **separate, configure-only** tree `build-uml/` with `-G Ninja`, purely to produce the database; nothing is compiled. It reuses `build-debug/vcpkg_installed` (via `VCPKG_INSTALLED_DIR` + `VCPKG_MANIFEST_INSTALL=OFF`) so Qt/Eigen/spdlog are not installed a second time, and calls `vcvars64.bat` when `VCINSTALLDIR` is unset, which puts `cl.exe` and VS's bundled ninja on `PATH` and — importantly — sets `INCLUDE`, which is how clang finds the MSVC and Windows SDK headers.

That last point matters because the installed clang-uml (0.6.1, `C:\Program Files\clang-uml`) ships **only** `bin\clang-uml.exe` — there is no `lib\clang\<ver>\include` resource directory, so clang's builtin headers have to come from MSVC's own copies.

The database is generated with **`cl.exe`** by default, since clang-uml 0.6.1's changelog lists "Fixed MSVC compilation flags" and that avoids requiring a second toolchain. If that turns out not to cover this project, there are two fallbacks, in order of cost:

- `--rewrite-db` runs `scripts/msvc-db-to-clang.ps1`, which replaces each entry's compiler token with `clang-cl.exe --driver-mode=cl` and strips `/showIncludes`. **No clang-cl binary needs to exist** — clang derives its flag dialect from argv[0] of the command, and the tooling never executes the compiler, so the name alone is enough to stop `/`-style flags from being parsed as GCC-style ones. The script is idempotent and writes BOM-less UTF-8 (clang's JSON parser rejects a BOM).
- `--clang-cl` configures with clang-cl as `CMAKE_CXX_COMPILER` instead. This machine has no clang-cl: VS 2022 Community ships only `clang-format`/`clang-tidy` under `VC\Tools\Llvm`, VS 18 Community has no `Llvm` directory at all, and there is no `C:\Program Files\LLVM`. It needs the VS component "C++ Clang tools for Windows" or a standalone LLVM install.

Nothing in the project uses AUTOUIC-generated `ui_*.h` or `#include "*.moc"`, so configuring without building is sufficient — there are no generated headers the parse would miss.

For rendering, the script prefers `PLANTUML_JAR`, then a `plantuml` launcher on `PATH`, then the Microsoft Store package **WinPlantUml** (`50760EliasAE.PlantUml`), which is what this machine has. That package is self-contained — `Java\plantuml.jar` plus a private JRE at `Java\jre\x64\bin\java.exe` and its own Graphviz at `Graphviz\bin\dot.exe` — so no separate Java install is needed. Its install path embeds the package version and `C:\Program Files\WindowsApps` cannot be enumerated by a normal user (a known full path *inside* it reads fine), so the script asks `Get-AppxPackage` for `InstallLocation` instead of globbing. Its `WinPlantUml.exe` is a GUI and registers no command-line alias, so it is not used.

`USE_QT` is defined in every diagram. The FLTK sources that used to be filtered out of the database no longer exist at all.

Diagrams are filtered by namespace (`fp`, `ivf2d`, `calfem`) where that works, and by explicit `include: elements:` lists for the Qt UI classes, which live in the global namespace mixed in with all of Qt. `overview` and `paintview_bridge` suppress members entirely (`exclude: access: [public, protected, private]`) — `fp::PaintView` alone has a few hundred methods and drowns out the structure otherwise.

## Architecture

### The single UI target

| Target | Directory | UI framework | Preprocessor |
|---|---|---|---|
| `qtforcepad` | `src/qtforcepad/` | Qt6 + QOpenGLWidget | `USE_QT`, `QT_NO_KEYWORDS` |

It links the static libraries `common`, `ivf2d`, `calfem`, `fplog`. `USE_QT` is now unconditionally defined, so any surviving `#ifndef USE_QT` branch is dead code.

### The `fp::PaintView` bridge pattern

`fp::PaintView` (`src/paintview/PaintView.h/cpp`) is the core logic class — ~3600 lines of framework-agnostic C++. It owns the FEM grid, brush/drawing state, OpenGL rendering, and event dispatch. It declares virtual methods for all UI interactions:

```cpp
virtual int height(); virtual int width();
virtual void doRedraw(); virtual void doMakeCurrent();
virtual const std::string doSaveDialog(...);
virtual bool doNewModel(int &w, int &h, int &initialStiffness);
// etc.
```

The UI port provides a concrete subclass inheriting from both a framework widget and `fp::PaintView`:

- **Qt6**: `QtPaintView : public QOpenGLWidget, public fp::PaintView` (`src/qtforcepad/QtPaintView.h/cpp`)

The bridge pattern is retained (it is what keeps `PaintView` portable to the wasm target), but with FLTK gone there is only one implementation, so new UI-specific behaviour goes straight into `QtPaintView` — no `#ifndef USE_QT` guard is needed.

### Shared library: `common`

Contains the FEM data model and purely algorithmic code: `FemGrid2`, `Node`, `Element`, `Force`, `Constraint`, `ColorMap`, `ImageGrid2`, `ForcePadClipboard`, etc.

**Important**: `FemGridSolver2.cpp` is excluded from the `common` library glob and compiled directly into the executable target, because it calls `QCoreApplication::processEvents()` and so needs the UI framework. (Before the FLTK retirement this was an `#ifndef USE_QT` fork against `Fl::check()`; only the Qt branch remains.) Do not add it back to the glob.

All `Fl_*`/`fl_*` sources, `LogWindow`, `NewModelDlg`, the v1 `FemGrid`/`ImageGrid` and `src/common/obsolete/` were deleted with the FLTK retirement, so `common` no longer carries any FLTK code.

### FEM solver

`fp::FemGridSolver2` (`src/common/FemGridSolver2.h/cpp`) assembles and solves the 2D plane-stress FEM problem using `calfem` (CALFEM C++ port) and Eigen for matrix algebra. It also runs topology optimisation (Sigmund filter). It is compiled separately into the executable because it calls `QCoreApplication::processEvents()`.

### Logging: `fplog` library

`src/fplog/` is a thin spdlog wrapper.

- **`FPLog.h`** exposes `fp_debug/fp_info/fp_warn/fp_error(ctx, fmt, ...)` macros (fmt-style) and a `FPLog::init()` function that wires up sinks.
- **`FPLog.cpp`** sets up a console sink by default; callers can pass additional sinks (e.g. a Qt widget sink) to `FPLog::init()`.
- **`so_print` compatibility**: `FPLog.h` defines `so_print(ctx, msg)` as `spdlog::info(...)` so legacy calls still compile. The `LogWindow.h` that used to compete for that name is gone.

**Qt log sink** (`src/qtforcepad/QtLogSink.h`): a `spdlog::sinks::base_sink` subclass that appends colour-coded messages to a `QPlainTextEdit` via `QMetaObject::invokeMethod(..., Qt::QueuedConnection)`, making it safe to call from the solver thread. It is instantiated in `MainWindow` and passed to `FPLog::init()` at startup.

### FEM solver logging

`FemGridSolver2.cpp` includes `FPLog.h` and uses `fp_debug/fp_info/fp_warn` throughout instead of bare `so_print`. It also has a local `#define NOMINMAX` before `<windows.h>` on WIN32 to guard against the `min`/`max` macro conflict independently of the global CMake definition — now wrapped in `#ifndef NOMINMAX`, since the unguarded define collided with the global `-DNOMINMAX` and warned (C4005).

### OpenMP and Eigen

OpenMP is enabled via `find_package(OpenMP COMPONENTS CXX)` in the root `CMakeLists.txt` (optional — build succeeds without it). `OpenMP::OpenMP_CXX` is linked into `calfem`, `common`, and `qtforcepad`, each gated on the aggregate `OpenMP_FOUND`. Eigen detects the `-fopenmp` / `/openmp` flag via `_OPENMP` and parallelizes dense matrix products and some decompositions automatically — no source changes needed.

**Windows/MSVC gotcha**: `<windows.h>` defines `min` and `max` as macros by default, which breaks `std::min`/`std::max` calls inside Eigen's `Parallelizer.h` when OpenMP is active. The root `CMakeLists.txt` defines `NOMINMAX` globally on Windows to prevent this. Do not remove it.

**macOS gotcha**: AppleClang ships no OpenMP runtime, so plain `find_package(OpenMP)` always fails on macOS even with `libomp` installed — it needs the separate frontend flag/library pointed at explicitly. The root `CMakeLists.txt` retries with hints toward Homebrew's `libomp` (`/opt/homebrew/opt/libomp` or `/usr/local/opt/libomp`) when the plain lookup fails; if that directory doesn't exist (`brew install libomp` not run), it's a no-op and the solver/Eigen fall back to single-threaded, which is a common reason a macOS build feels slower than the Windows/Linux ones for the same model. Note the `COMPONENTS CXX` on the `find_package(OpenMP ...)` call is load-bearing: the project enables both C and CXX languages by default, so an unqualified `find_package(OpenMP)` requires the *C* frontend's OpenMP support too — the Homebrew-libomp hint above only patches the CXX-side variables, so without `COMPONENTS CXX` the aggregate `OpenMP_FOUND` silently stays false (even once `libomp` is installed and `OpenMP_CXX_FOUND` is true) and nothing ends up linking OpenMP at all, since every consumer gates on the aggregate. Verify with `otool -L` on the built binary for `libomp.dylib` if you ever need to confirm it's actually engaged, rather than trusting the CMake status lines alone.
