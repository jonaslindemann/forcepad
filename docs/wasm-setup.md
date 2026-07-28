# ForcePAD WebAssembly — setup & build

**Step 2** of the web port. Step 1 (the fixed-function → modern-GL rendering
migration) is complete; Step 2 stands up the Qt-for-WebAssembly toolchain and a
wasm build target.

**Status (2026-07-26): builds and renders in the browser.** ForcePAD compiles to
WebAssembly and runs in Chrome/Edge on a **WebGL 2 (OpenGL ES 3.0)** context —
full UI (menus, tool palette, Properties panel), the drawing canvas, and the
`Renderer2D` shader pipeline all render. Target kit: **`wasm_singlethread`**
(hosts on any static server; the solver blocks the UI as on desktop). Remaining
work: async file open/save (browser sandbox), then the multi-threaded/worker
solver.

---

## Toolchain (installed & verified)

| Component | Version | Notes |
|---|---|---|
| Qt (wasm kit) | **6.9.3 `wasm_singlethread`** | `E:\Qt\6.9.3\wasm_singlethread` — installed via the Qt Maintenance Tool |
| Emscripten | **3.1.70** | Qt 6.9 hard-pins this; `E:\Users\Jonas\Development\emsdk` |
| Ninja | 1.13.2 | on PATH |
| Desktop Qt (unchanged) | vcpkg Qt | wasm uses the official kit; the two coexist |

Qt ↔ Emscripten pin (re-check at `https://doc.qt.io/qt-6/wasm.html` if Qt is
upgraded): 6.8 → 3.1.56 · **6.9 → 3.1.70** · 6.10/6.11 → 4.0.7.

### If setting this up on a fresh machine

1. **Qt WebAssembly kit** — run `E:\Qt\MaintenanceTool.exe` → *Add or remove
   components* → under **Qt 6.9.x** tick **WebAssembly (single-threaded)**. (It's
   a sibling of the MSVC/MinGW kits in the tree, easy to miss; sign in first, and
   the CLI `MaintenanceTool.exe install qt.qt6.<ver>.wasm_singlethread` works too.)
2. **Emscripten 3.1.70:**
   ```
   cd E:\Users\Jonas\Development\emsdk
   git pull                                    # refresh release tags if old
   python emsdk.py install 3.1.70              # needs Python >= 3.10
   python emsdk.py activate 3.1.70
   ```
   Gotchas learned the hard way: the bundled emsdk Python may be too old
   (< 3.10) — drive `emsdk.py` with a system Python 3.12; and an old emsdk
   checkout won't know 3.1.70 until `git pull`.
3. Verify: `emcc --version` → `3.1.70`;
   `Test-Path E:\Qt\6.9.3\wasm_singlethread\bin\qt-cmake.bat` → True.

---

## Build & run

Two convenience scripts wrap the emsdk env + Qt `qt-cmake`:

```powershell
pwsh scripts/wasm-build.ps1                        # Debug -> bin/wasm/
pwsh scripts/wasm-build.ps1 -Config MinSizeRel     # the one to deploy (~15 MB)
pwsh scripts/wasm-serve.ps1                        # static server on :8137 + open browser
# then browse to http://localhost:8137/ForcePAD.html
```

`-Config` takes `Debug` (default), `Release`, `RelWithDebInfo` or `MinSizeRel`.
Ninja is single-config, so each build type gets its own tree — `build-wasm/` for
Debug and `build-wasm-rel/` for MinSizeRel, the names those trees already had.
**All of them write the app to `bin/wasm/`, so the last build wins there**: run
the MinSizeRel build immediately before deploying, or `bin/wasm` still holds a
Debug app. (`wasm-deploy.ps1` refuses to publish one, which is the backstop.)

A third script publishes a build to the documentation site:

```powershell
pwsh scripts/wasm-deploy.ps1        # bin/wasm -> docs/docs/app (served at /forcepad/app/)
```

Each of the three scripts has a `.cmd` twin for plain `cmd.exe`
(`wasm-deploy.cmd force prunelegacy <model.fp2> …` — same behaviour, positional
arguments instead of named ones).

It copies the payload and writes `index.html` from the generated `ForcePAD.html`
in one step, so the page can never end up asking for a payload that is no longer
there — the way a partial copy breaks the site. It also publishes
`bin/release/samples/*.fp2` as `docs/docs/app/models/`, so every sample is
reachable as `?model=models/<name>` (add one-offs with `-Models`, skip the lot
with `-NoModels`). It refuses a Debug wasm (63 MB against 14.8 MB for
MinSizeRel) unless `-Force`, and leaves the previous release's `qtforcepad.*`
files alone until `-PruneLegacy`.

`wasm-build.ps1 -Clean` forces a fresh configure of that config's tree. Under
the hood it runs `E:\Qt\6.9.3\wasm_singlethread\bin\qt-cmake.bat -G Ninja
-DCMAKE_BUILD_TYPE=<config>` after sourcing `emsdk_env.ps1`. The servable app
(html + js + wasm + data + `qtloader.js` + `qtlogo.svg`) lands in `bin/wasm/`.

> Measured sizes: Debug **63 MB** (`-g`, instantiates in a few seconds locally),
> MinSizeRel **14.8 MB** (`-Os`) — the one to serve.

---

## How the wasm build differs (what the CMake does)

No vcpkg on wasm. The `EMSCRIPTEN` branch in the top-level `CMakeLists.txt`
includes `cmake/WasmDeps.cmake`, which provides:

- **Eigen3** — header-only, FetchContent (pinned 3.4.0).
- **spdlog** — FetchContent + compiled for wasm (pinned 1.17.0).
- **zlib / libpng / libjpeg** — **Emscripten ports** (`-sUSE_ZLIB/USE_LIBPNG/
  USE_LIBJPEG`), exposed as `ZLIB::ZLIB` / `PNG::PNG` / `JPEG::JPEG` INTERFACE
  targets so the rest of the build links them by their usual names. (`ivf2d`
  links all three because `PngImage.cpp`/`JpegImage.cpp` need the headers.)

Other wasm-specific bits:

- The per-library `find_package()` calls are guarded `if(NOT TARGET …)` so the
  WasmDeps targets satisfy them.
- `qtforcepad` (wasm): no system `OpenGL::GL`, no OpenMP; link options
  `-sMAX_WEBGL_VERSION=2` (WebGL 2 for the `#version 300 es` shaders),
  `-sALLOW_MEMORY_GROWTH=1`, and `--preload-file images/svg@/icons` (bakes the
  toolbar SVGs into MEMFS at `/icons`, where the app looks). `OUTPUT_NAME` and
  `QT_WASM_EXPORT_NAME` are both set to `ForcePAD`, which is what
  `Qt6WasmMacros.cmake` substitutes for `@APPNAME@` / `@APPEXPORTNAME@` when it
  generates the shell from `wasm_shell.html` — so the page title, the artifact
  names and the entry-point call all agree. Note the entry point is
  `window.ForcePAD`, not `ForcePAD_entry`: `QT_WASM_EXPORT_NAME` is used verbatim
  (Qt only appends `_entry` when the property is unset) and is also what goes to
  emscripten as `-sEXPORT_NAME`, so `ForcePAD.js` defines exactly that symbol and
  the shell calls exactly that. `DEBUG_POSTFIX` is
  cleared (that macro does *not* see it, so a postfix would desync the generated
  `ForcePAD.html` from a `ForcePADd.js`) and output is unified into `bin/wasm/`.
- **`main.cpp`**: on `Q_OS_WASM` the `QSurfaceFormat` requests **OpenGL ES 3.0 /
  NoProfile** (not desktop 3.3 Core). A Core-profile request makes
  `QOpenGLWidget` fail to create a WebGL context → blank canvas. This was the
  key runtime fix.

---

## Working in the browser

- Rendering, tool palette, **drawing (brush)**, forces/constraints, running the
  solver.
- **Dialogs** — New Model, Settings (Calc/General), About. They use non-blocking
  `open()`+signals on wasm (blocking `exec()` stays on desktop); Emscripten
  Asyncify was tried for this and rejected (it conflicts with Qt's wasm event
  loop). See the note in `src/qtforcepad/CMakeLists.txt`.
- **File open/save** — models load via `QFileDialog::getOpenFileContent` (bytes
  staged to a `/tmp` MEMFS file, then the normal path-based loader runs); save
  triggers a browser download via `saveFileContent`. Implemented as continuation-
  passing `doNewModel`/`doPickFile`/`doSaveModelFile` hooks so desktop stays
  synchronous.

### Opening a model from a link (`?model=`)

A link can open a predefined model, so a course page can hand out one URL per
exercise:

```text
https://jonaslindemann.github.io/forcepad/app/?model=models/beam.fp2
https://jonaslindemann.github.io/forcepad/app/?model=https://example.org/beam.fp2
```

The value is resolved against the page URL, so a relative path is served from
next to the app (`docs/docs/app/models/…` in this repo's published site). An
absolute URL to another host only works if that host sends CORS headers
(`Access-Control-Allow-Origin`) — the transfer is a plain XHR from the page.
Only `http`/`https` are accepted; anything else is refused and logged.

`src/qtforcepad/ModelUrlLoader.cpp` implements this. On wasm it reads
`window.location.href` via `emscripten_run_script_string` (the browser's only
"command line"), picks the `model` query parameter apart with `QUrlQuery`, and
downloads with `emscripten_async_wget2` straight into `/tmp` — from there the
normal path-based `openModel()` takes over, exactly like the file-dialog path.
Using emscripten's XHR rather than `QNetworkAccessManager` keeps `Qt6::Network`
out of the wasm module. Failures (404, CORS, unsupported scheme) leave the app
on an empty model and report through the info overlay and the log.

The same helper works on desktop, where it links `Qt6::Network` instead, so
`qtforcepad https://example.org/beam.fp2` downloads and opens a model too. A
plain path argument still takes the existing local-file route.

## Still to do

1. **Threaded solver** — move to `wasm_multithread`, run the solver on a worker
   thread (OpenMP/pthreads), and serve with COOP/COEP cross-origin-isolation
   headers so `SharedArrayBuffer` is available. This also unblocks the
   optimisation dialog (`runOptimise`), which still blocks because it gates a
   long solver loop.
2. ~~**Release build** to shrink the ~66 MB debug wasm, and a deployment host.~~
   Done: `wasm-build.ps1 -Config MinSizeRel` (14.8 MB) published to GitHub Pages
   at `/forcepad/app/` by `wasm-deploy.ps1`.
