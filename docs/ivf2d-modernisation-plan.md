# ivf2d Modernisation Plan — removing the legacy Ivf framework layer

**Goal:** turn `src/ivf2d/` from a late-1990s C++ framework (custom macro RTTI,
intrusive reference counting, `windows.h` in the base header, `TRUE`/`FALSE`,
raw `char*`/`new[]`) into plain modern C++17 — without changing rendering
behaviour, which was already modernised by the
[rendering migration](rendering-migration-plan.md).

That earlier migration replaced *what the library draws with*
(`Renderer2D`/`StreamTexture`/`GLProgram`/`Mat4` are already modern code). This
plan replaces *how the library is written*.

---

## 1. Current state

24 classes across 50 files. Two clearly separated strata:

| Stratum | Files | State |
|---|---|---|
| Modern GL layer (added 2026-07) | `Mat4.h`, `GLProgram.*`, `Renderer2D.*`, `StreamTexture.*` | Already modern — in-class initialisers, `unique_ptr`, deleted copies, `enum`, no macros. **Leave alone.** |
| Legacy framework | everything else (`Base`, `DrawableBase`, `Shape`, `Image`, `Color`, `Line`, `Ellipse`, `Rectangle`, `Arch`, `Clipboard`, `ScreenImage`, image loaders, …) | Macro-driven, raw-pointer, C-string, `windows.h`-leaking. **This plan's target.** |

Ownership has *already* been half-migrated: every class has a static
`create()` returning `std::shared_ptr`, and `IvfSmartPointer(X)` now expands to
`std::shared_ptr<X>`. What remains is the scaffolding around that.

### 1.1 What the macros actually are

Three headers define them; only one is live:

- **`CommonDefs.h`** — the live one. Included by `Base.h`, therefore by
  effectively everything in `ivf2d`, `common`, `paintview` and both UI ports.
- **`IvfDef.h`** — dead. Never included by anything; still `#include
  <ivf/ivfconfig.h>`, a path that does not exist in this repo, which proves it
  is never compiled.
- **`IvfPointer.h`** — dead. Defines `CIvfPointer<T>`, the *intrusive
  reference-counted* smart pointer, calling `addReference()`/`delReference()`/
  `isReferenced()`. **No class in the repo defines those methods any more** —
  the refcounting base was already removed when `create()`/`shared_ptr` came in.
  The header is orphaned and cannot compile if included.

### 1.2 The RTTI macros are pure dead weight

`IvfClassInfo(name, parent)` / `IvfClassInfoTop(name)` inject
`getClassName()`, `getClassNameThis()` and `isClass()` into 38 classes
(76 macro invocations, `ivf2d` + `common` + `forcepad`).

**There is not a single call site.** A repo-wide grep for `isClass(`,
`getClassName` and `getClassNameThis` finds hits *only* inside the macro
definitions themselves and inside the dead `IvfPointer.h`. Nothing dispatches
on class name; nothing prints it.

They are also actively unsafe:

```cpp
virtual void getClassName(char* name) { strcpy(name, ivfclassname); } // no size
virtual bool isClass(char* name) {
    char className[30] = "";        // silent truncation past 29 chars
    getClassNameThis(className);
    if (className != NULL) { ... }  // an array is never NULL — always true
```

plus a `#if defined(__APPLE__) || !defined(WINDOWS)` fork purely to pick
between `strcpy` and `strcpy_s`. (Note `WINDOWS`, not `_WIN32` — the MSVC
branch is likely never taken anyway.)

**Conclusion: delete outright.** No replacement is needed. If class identity is
ever wanted later, `typeid` / `dynamic_cast` already work — the classes are
polymorphic.

### 1.3 `CommonDefs.h` is the real problem

Beyond the macros, this header — reached from *every* translation unit that
touches `Base.h` — does:

```cpp
#define WIN32_LEAN_AND_MEAN
#include <windows.h>       // "ripped from the GLUT 3.7 header file"
#include <mmsystem.h>
#define TRUE 1
#define FALSE 0
#define NULL 0
#define M_PI 3.14159...
#pragma warning(disable : 4786)  // VC6-era template-name-truncation warning
#pragma warning(disable : 4244)  // "bogus conversion warnings"
#pragma warning(disable : 4305)
#pragma warning(disable : 4800)
```

This is the highest-value removal in the plan. It is the mechanism behind
several hazards already documented in `CLAUDE.md` — the `NOMINMAX` requirement,
the `UNICODE`/TCHAR sensitivity of `SystemInfo.cpp`, and the need to link Qt
`PRIVATE` into `ivf2d`. Pulling `windows.h` into a 2D geometry library is
gratuitous; `mmsystem.h` (multimedia timers) is never used at all. The
blanket-disabled conversion warnings hide real narrowing bugs across the whole
codebase.

### 1.4 Dead classes

- **`Arrow`** — `Arrow.cpp` is an empty constructor and empty destructor. The
  class declares three raw `Line*` members that are never allocated, never
  freed, and never read. Zero uses outside `ivf2d`. Delete.
- **`Texture`** — a fixed-function-era GL texture object. `Texture::create()`
  is never called, `bind()`/`apply()` are never called, and no `setTexture()`
  call site exists anywhere. It survives only as an unused `TexturePtr` member
  in `Shape` plus a `Rectangle::setTexture` override that nothing invokes.
  Superseded by `StreamTexture`. Delete (it is also the last file in `ivf2d`
  that pulls in `<GL/gl.h>` for real work).
- **`Makefile.am`** — autotools relic, not referenced by CMake.
- **`force_config.h`** — an autoconf-generated `config.h` from ~2001 defining
  `HAVE_CMATH` etc. It is *syntactically broken*: it ends with an unclosed
  `namespace ivf2d {`. Its one includer is `common/fl_file_chooser_win.cpp`,
  which is FLTK-only.
- **`Base::setParent`/`getParent`** — the parent pointer is written and read
  nowhere outside `Base` itself.

### 1.5 Remaining C-isms

| Site | Issue |
|---|---|
| `SgiImage.h:m_fileName` | `char*` with `new char[strlen(name)+1]` / `delete[]`, while the sibling `JpegImage`/`PngImage` already use `std::string` |
| `Image.cpp` | layer storage is `unsigned char**` + manual `new[]`/`delete[]` loops |
| `Base.h:30` | `#pragma comment(lib, "opengl32")` / `"glu32"` — linking belongs in CMake, and `glu32` is no longer used at all |
| `Image.cpp`, `Point.cpp` | still `#include <GL/gl.h>` although they no longer make GL calls |
| `Vec3d`, `Point`, `Shape` | `double m_pos[2]` with out-parameter getters (`getPosition(double&, double&)`), non-`const` accessors, `virtual` without `override` |
| `SingletonDestroyer.h` | 1996-vintage "To Kill a Singleton" template; used by `common/UiSettings` and `forcepad/PlatformInfo`. A function-local `static` (Meyers singleton) does this correctly and thread-safely since C++11 |

---

## 2. Scope decisions

Two things are unavoidable and worth stating up front:

1. **This cannot stay inside `src/ivf2d/`.** `IvfClassInfo` is used by 17
   classes in `src/common/` and 2 in `src/forcepad/`, and `IvfSmartPointer`
   likewise. Removing the macros necessarily edits those files. The edits are
   mechanical (delete a line / replace a line), but the blast radius is
   ~38 files.
2. ~~**The FLTK target (`BUILD_FLTK_APP=ON`) is treated as best-effort.**~~
   **Superseded 2026-08-01: FLTK has been retired entirely.** See
   "FLTK retirement" below. Qt is now the only target.

Verification gates for every phase: **`qtforcepad` desktop build green** + app
launches and renders a sample model. The wasm build is re-verified once, at the
end (Phase 8) — nothing here is platform-specific, but the preload/asset paths
make it a slow gate to run per-phase.

There are no automated tests in this repo; verification is build + the
screenshot recipe already used for the rendering migration
(`Start-Process` → `Start-Sleep` → `System.Windows.Forms.Screen` capture →
read the PNG), driven against `bin/Debug/samples/thick_beam.fp2`.

---

## 3. Phases

Each phase leaves the desktop build green and is a self-contained commit.
Ordered lowest-risk-first: phases 1–3 are provably behaviour-neutral deletions.

### Phase 1 — Delete dead files (zero risk) — **DONE (2026-08-01)**

Deleted `IvfDef.h`, `IvfPointer.h`, `Makefile.am`, `Arrow.h`, `Arrow.cpp`,
`force_config.h`. The one `#include <force_config.h>` in
`common/fl_file_chooser_win.cpp` sat behind `#ifdef HAVE_CONFIG_H`, which is
never defined, so the file already took the `#include <stdio.h>` branch of the
`HAVE_CSTDIO` fork below it — both dead conditionals were replaced with a plain
`#include <stdio.h>`, leaving preprocessed output identical.

`Makefile.am` also referenced `src/newmat09`, a library no longer in the repo —
further confirmation it had not been used in a very long time. Note that
`src/forcepad_r/Makefile.am` and the `bin/*/` ones still exist; only the
`ivf2d` one was in scope here.

Verified: both `build-debug` (Debug) and `build-release` (Release) build clean.

The `CMakeLists.txt` uses `file(GLOB ... CONFIGURE_DEPENDS)`, so no build-file
edits were needed for the removed sources.

### Phase 2 — Delete the RTTI macros (mechanical, 33 files)

1. Delete the four `IvfClassInfo*` definitions from `CommonDefs.h`.
2. Delete all `IvfClassInfo(...)` / `IvfClassInfoTop(...)` invocation lines
   across `ivf2d/`, `common/`, `forcepad/`.

No call sites exist, so this is a compile-or-not proposition — if it builds, it
is correct.

**Verify:** build; grep that `IvfClassInfo` returns nothing.

**DONE (2026-08-01).** 32 invocation lines removed across 32 files (14 in
`ivf2d/`, 17 in `common/`, 1 in `forcepad/`), plus the whole
`#if defined(__APPLE__) || !defined(WINDOWS)` … `#endif` block in
`CommonDefs.h` holding all four definitions. Net −148 lines.

The count is 32, not the 76 estimated when the plan was written — that earlier
figure came from a grep matching `IvfClassInfo|IvfSmartPointer` together, so it
double-counted the pointer macros that Phase 3 handles. `CommonDefs.h` is now
down to just the two pointer-typedef macros.

Repo-wide grep for `IvfClassInfo`, `getClassNameThis` and `isClass(` returns
nothing, confirming the "no call sites" analysis.

### Phase 3 — Replace the pointer-typedef macros

Replace each `IvfSmartPointer(X);` with an explicit, greppable declaration in
the same header:

```cpp
class Shape;
using ShapePtr = std::shared_ptr<Shape>;
```

Keeping the `XPtr` names means **no consumer changes** — `common`,
`paintview` and both UI ports are untouched by this phase.

`IvfStdPointer(X)` (`X*` typedef) has two uses, both singletons
(`common/UiSettings.h`, `forcepad/PlatformInfo.h`); inline them the same way.
The `XStdPtr` alias that `IvfSmartPointer` also emitted has **zero** uses
repo-wide — drop it silently.

Then delete `IvfSmartPointer`/`IvfStdPointer` from `CommonDefs.h`.

**Verify:** build.

**DONE (2026-08-01).** 35 sites expanded — 33 `IvfSmartPointer` (14 `ivf2d`,
18 `common`, 1 `forcepad`) and 2 `IvfStdPointer` (`common/UiSettings.h`,
`forcepad/PlatformInfo.h`). The `XStdPtr` alias was dropped as planned; a
repo-wide grep confirmed zero uses.

Each header that gained a `std::shared_ptr` alias also gained
`#include <memory>` if it lacked one (32 of the 35 did) — previously they were
relying on `CommonDefs.h` including it transitively via `Base.h`, which stops
working the moment Phase 4 deletes that header. Doing it now keeps Phase 4
focused on the `windows.h`/`TRUE`/`FALSE` removal rather than chasing missing
`<memory>` includes. Placement was verified programmatically: in every file the
new include sits in the leading include block, ahead of the first `namespace`
or `class` declaration.

Both macro definitions were then removed from `CommonDefs.h`, so a repo-wide
grep for `IvfSmartPointer|IvfStdPointer|IvfClassInfo` now returns zero. The
header no longer defines a single macro of its own — only the `windows.h`
include, the `M_PI`/`TRUE`/`FALSE`/`NULL` defines and the warning pragmas that
Phase 4 targets remain.

No consumer code changed, as intended — the `XPtr` names are identical.

Cumulative for phases 1–3: 38 files modified (+103/−183) and 6 files deleted
(−555). Verified: Debug and Release both build with zero errors and the single
pre-existing `NOMINMAX` C4005 warning in `FemGridSolver2.cpp`.

### Phase 4 — Dismantle `CommonDefs.h` (highest value)

Now that only the non-macro cruft is left:

1. Replace the 8 `TRUE`/`FALSE` uses with `true`/`false`; drop the defines.
   (`Texture.cpp`'s `if (isBound() == TRUE)` disappears with Phase 6 anyway.)
2. Drop the `NULL` define (C++11 has `nullptr`; the sources already mostly use
   it).
3. `M_PI` — used in 10 files. Define once as
   `inline constexpr double PI = 3.14159265358979323846;` in a small
   `ivf2d/Constants.h`, or keep `_USE_MATH_DEFINES` + `<cmath>` locally in the
   files that need it. Prefer the former; `M_PI` is not standard C++.
4. Delete `#include <windows.h>` / `<mmsystem.h>` and the GLUT-derived
   `APIENTRY`/`WINGDIAPI` block. Anything that genuinely needs Win32 must
   include it itself, with `NOMINMAX`/`WIN32_LEAN_AND_MEAN` — expect a few
   fallout compile errors in `common/` here; that is the point.
5. Remove the four `#pragma warning(disable: ...)`. Expect a wave of C4244
   narrowing warnings — **do not re-disable them**; note them and fix in
   Phase 7, or scope them narrowly to the offending files as an interim step.
6. `Base.h`: delete the `#pragma comment(lib, "opengl32"/"glu32")` block; add
   `OpenGL::GL` to `ivf2d`'s `target_link_libraries` if the link then fails
   (it likely will not — Qt6::OpenGL already brings it).
7. Trim `Base.h`'s include list (`<iostream> <cmath> <cstdlib> <cstring>
   <cstdio> <vector> <stack> <deque> <set>`) to what `Base` actually needs
   (`<iosfwd>`, `<memory>`), pushing the rest down into the `.cpp`s that use
   them.
8. Delete `CommonDefs.h`.

This is the riskiest phase for *compile* breakage (and the most valuable), but
still behaviour-neutral. Do it as one commit so a bisect lands cleanly.

**Verify:** build with warnings visible; run the app; record the new warning
count as a baseline.

**DONE (2026-08-01).** Final state: clean rebuild of both configs, 57
translation units, **0 errors and 0 warnings** — cleaner than the pre-phase
baseline, which carried a `NOMINMAX` C4005 warning.

**Correction to section 1.3 above: the GLUT-derived `windows.h` block was
*not* gratuitous.** Its own comment was accurate — Win32's `<GL/gl.h>` needs
`APIENTRY`/`WINGDIAPI`, which come from `<windows.h>`. Deleting it broke every
`ivf2d` translation unit that included legacy GL (~220 errors inside
`GL/gl.h`), then `qtforcepad` on `<GL/glu.h>`. The block was load-bearing for
the legacy GL includes, and the plan was wrong to dismiss it.

The fix was not to restore it, though: the errors confirmed the deeper
analysis. Of the five files that depended on the implicit include, **three
did not need GL at all**:

| File | Depended on | Resolution |
|---|---|---|
| `paintview/PaintView.cpp` | `windows.h` for `HGLOBAL`, `IsClipboardFormatAvailable`, `CF_DIB`, `GetModuleFileNameA` | explicit `<windows.h>` + `NOMINMAX`/`WIN32_LEAN_AND_MEAN` |
| `ivf2d/Image.cpp` | `windows.h` for `<GL/gl.h>` | GL/GLU includes deleted — **zero** GL calls |
| `ivf2d/Point.cpp` | same | GL/GLU includes deleted — **zero** GL calls |
| `ivf2d/Texture.cpp` | same | explicit `<windows.h>`; 13 real GL calls. Goes away entirely in Phase 6 |
| `qtforcepad/qtpaintview.cpp` | `windows.h` for `<GL/glu.h>` | include deleted — **zero** GLU calls; all 17 GL entry points go through `QOpenGLExtraFunctions` |

Net effect: one implicit project-wide `<windows.h>` became two explicit,
justified includes in the only files that genuinely need Win32.

**The predicted C4244 warning flood did not happen.** Removing
`#pragma warning(disable: 4244/4305/4800)` unmasked *nothing* at MSVC's default
`/W3` (no `CMakeLists.txt` sets a warning level). The two warnings that did
appear were pre-existing and unrelated, and both are now fixed:

- C4005 `NOMINMAX` redefinition (`FemGridSolver2.cpp`) — local `#define`
  colliding with the global `-DNOMINMAX` at `CMakeLists.txt:153`; now wrapped
  in `#ifndef`, keeping the deliberate local guard without the collision.
- C4091 `typedef ignored` (`SystemInfo.h`) — C-style `typedef enum
  WindowsVersion {…}` declaring no typedef name; the redundant `typedef` was
  dropped (the enum tag is already a type name in C++), leaving all four
  usages unchanged.

The most likely explanation for the missing flood is that those pragmas were
shielding the v1 `FemGrid.cpp`/`ImageGrid.cpp` and other FLTK sources that the
Qt build stopped compiling during the rendering migration. **Consequence for
Phase 7: its "fix the narrowing warnings unmasked in Phase 4" item is empty as
written.** Those narrowing bugs still exist but are invisible at `/W3`;
surfacing them needs the `/W4` step in Phase 8.

Also done here: `M_PI` replaced by `ivf2d::PI` from the new `ivf2d/Constants.h`
(33 replacements across 8 files, including the removal of `Vec3d.cpp`'s own
duplicate `#ifndef M_PI` fallback), and `Base.h` lost its
`#pragma comment(lib, "opengl32"/"glu32")` block.

Only 2 of the 8 `TRUE`/`FALSE` uses actually came from `CommonDefs.h`
(`Texture.cpp`'s `isBound() == TRUE`). The other 6 are genuine Win32 `BOOL` or
libjpeg values in files that include `<Windows.h>`/`jpeglib.h` themselves and
were correctly left alone.

**Deferred: item 7, trimming `Base.h`'s include list.** It still pulls
`<iostream> <cmath> <cstdlib> <cstring> <cstdio> <vector> <stack> <deque>
<set>`, which many `common/`/`paintview/` sources get transitively. Removing
them is a potentially wide cascade of unrelated compile errors for a purely
cosmetic gain, so it was left out rather than destabilise an otherwise green
phase.

**Known FLTK-only breakage:** `common/FemGrid.cpp`, `common/ImageGrid.cpp`,
`forcepad/FlPaintView.h` and `forcepad_r/PaintView.h` still include
`<GL/glu.h>`/`<GL/gl.h>` with no `<windows.h>`. All are excluded from the Qt
build, so they do not affect it, but a `BUILD_FLTK_APP=ON` build would now
fail there. Consistent with the section 2 decision that FLTK is best-effort;
the fix is two lines per file whenever that build is revived.

### FLTK retirement (2026-08-01, out of the original plan's scope)

Decided mid-way through the phases: Qt is the only target, so the legacy port
was removed outright rather than kept best-effort.

Build wiring: the `BUILD_FLTK_APP` option, the `find_package(FLTK)` call, the
FLTK include-directory branches, the `add_subdirectory(forcepad)` and the
FLTK source-exclusion block in `src/common/CMakeLists.txt` are all gone. FLTK
was never a vcpkg dependency, so `vcpkg.json` needed no change.

Sources deleted: `src/forcepad/` (56 files), `src/forcepad_r/` (38),
`src/forcepad_kiosk/` (1), `src/common/obsolete/`, and from `src/common/` all
`Fl_*`/`fl_*` sources, `LogWindow`, `NewModelDlg`, the `.fl` FLUID sources and
the v1 `FemGrid`/`ImageGrid`. `FemGridSolver2.cpp`'s `#ifndef USE_QT` fork
collapsed to the `QCoreApplication::processEvents()` branch.

**Gotcha worth remembering: `src/forcepad/forcepad_config.h` was not an FLTK
file.** It holds `FORCEPAD_VERSION_STRING`/`FORCEPAD_NAME` and is included by
`paintview/PaintView.h` and `qtforcepad/AboutDialog.cpp` — it merely *lived* in
the FLTK directory, reachable through `include_directories(../forcepad)` in the
Qt target. Deleting the tree broke the Qt build. It now lives in `src/defs/`
alongside `forcepad_defs.h`, which is already on the global include path, and
`../forcepad` was dropped from the Qt target's include list.

The lesson for any future tree deletion here: enumerate *every* header in the
tree and grep each against surviving code, rather than grepping for the
directory and a few class names. Doing that properly afterwards turned up only
one other reference — `MainFrame2.h`, inside a dead `#ifndef USE_QT` in
`PaintView.cpp`, since removed.

Still outstanding: `PaintView.cpp` retains other `#ifndef USE_QT` branches,
deferred to a dedicated pass. `USE_QT` is now always defined, so they are dead
code.

### Phase 5 — Slim `Base` and `DrawableBase`

After Phase 4, `Base` is: an unused parent pointer + two stream virtuals + a
virtual destructor.

- Remove `m_parent`/`setParent`/`getParent` (no users).
- Keep `readFromStream`/`saveToStream` — `common::Force` and
  `common::Constraint` override them, and `FemGrid2` calls through them.
- `= default` the destructor; delete the empty `Base()`/`~Base()` bodies in
  `Base.cpp` (the file then holds only the two empty stream stubs — consider
  making them pure-virtual-free inline defaults in the header and deleting
  `Base.cpp` entirely).
- Same treatment for `DrawableBase`: empty ctor/dtor → `= default`; mark
  `render()` and the four `do*()` hooks `virtual ... = 0`-or-default
  consistently, and add `override` in every subclass that is missing it
  (`Line::doGeometry`, `Ellipse::doGeometry`, `Arch::doGeometry`,
  `ScreenImage::doGeometry` all currently lack it).

**Open question worth deciding here:** whether `Base` should exist at all. It
is now a 20-line serialisation interface that 17 `common` classes inherit
purely by inertia. Recommendation: **keep it**, renaming nothing — removing it
churns ~17 files in `common` for no functional gain, and it does still carry
the polymorphic-destructor guarantee.

**DONE (2026-08-01).** `Base` kept, per the recommendation above.

- `m_parent`/`setParent`/`getParent` removed (no users anywhere).
- `Base()` and `~Base()` are now `= default`; `DrawableBase`'s empty
  constructor/destructor pair likewise, with `~DrawableBase() override`.
- `Base.cpp` shrank to just the two stream stubs. They are deliberately kept
  **out of line** rather than inlined into the header: as the class's key
  function they anchor the vtable to one translation unit instead of emitting
  it in every TU that includes `Base.h`.
- `override` added to the eight subclass hooks that lacked it — `doGeometry` in
  `Arch`, `Ellipse`, `ScreenImage`, `Line` (`ivf2d`) and `CGIndicator`,
  `ImageGrid2`, `ReactionForce`, `Ruler` (`common`). Two of those (`Line`,
  `Ruler`) had dropped `virtual` entirely and were overriding only by
  signature match. All confirmed genuine overrides — with `override` in place
  the compiler now enforces it.

Verified: clean configure + rebuild of both configs, 114 TUs total, 0 errors,
0 warnings.

Still deferred from Phase 4: trimming `Base.h`'s nine standard-library
includes.

### Phase 6 — Remove `Texture`, finish the GL cleanup

- Delete `Texture.h`/`Texture.cpp`.
- Remove `Shape::m_texture`, `Shape::setTexture`, `Shape::getTexture`, and
  `Rectangle::setTexture` (+ the now-unused `m_textureSetup`, `m_imageRatio`
  ratio bookkeeping in `Rectangle` if it is only texture-related — check).
- Drop the `#include "Texture.h"` from `Shape.h`, `Rectangle.h`, `Arch.h`.
- Delete the vestigial `<GL/gl.h>` / `<OpenGL/gl.h>` includes from `Image.cpp`
  and `Point.cpp`.

After this, **no file in `ivf2d` includes a GL header except the four modern-GL
files**, all of which go through `QOpenGLExtraFunctions`. That is a clean
invariant worth adding to `scripts/check-legacy-gl.sh`.

**Verify:** build + render a model with a rectangle/ellipse shape committed
(the marquee and shape-preview paths are what used `Shape`'s transform code).

**DONE (2026-08-02).** `Texture.h`/`Texture.cpp` deleted, along with
`Shape::m_texture`/`setTexture`/`getTexture` and the `Texture.h` includes in
`Shape.h`, `Rectangle.h` and `Arch.h` (`Arch` never referenced the type at all).

`Rectangle` lost more than the planned `setTexture` override: its
`m_ratioX`/`m_ratioY`/`m_imageRatio`/`m_textureSetup` members turned out to be
**write-only**. `m_textureSetup` and `m_ratioY` were assigned and never read
anywhere; `m_ratioX` and `m_imageRatio` were read only inside the
texture-setup block in `setSize()` that computed `m_textureSetup`. The whole
"Calculate ratios / Determine texture setup" block went with them, so
`setSize()` is now two assignments.

The GL-include cleanup this phase was supposed to do had already been pulled
forward by Phase 4 (`Image.cpp`, `Point.cpp`), so removing `Texture.cpp`
completed it: **`src/ivf2d` now contains no platform GL header at all.** The
only `<GL/gl.h>` left in the whole tree is `paintview/PaintView.cpp`, for the
core-safe `glReadPixels`/`glGetString`/`glPixelStorei` reads.

That invariant is now enforced: `scripts/check-legacy-gl.sh` gained a second
check that fails if any file under `src/ivf2d` includes `<GL/...>` or
`<OpenGL/...>`. Its stale exclusion for the v1 `FemGrid`/`ImageGrid` (deleted
with the FLTK port) was removed at the same time. The script passes both
invariants.

### Phase 7 — Memory and type hygiene inside `ivf2d`

Now behaviour-preserving but non-mechanical; do class by class, one commit each.

1. **`SgiImage`**: `char* m_fileName` → `std::string`, matching `JpegImage`/
   `PngImage`. Removes two `delete[]` paths and a `strlen`/`strcpy`.
   (Consider whether `SgiImage` is still needed at all — the brush masks are
   `.rgb` SGI files loaded by `PaintView::loadBrushes()`, so **yes, it is
   live**; do not delete it.)
2. **`Image`**: `unsigned char** m_imageMaps` + `initLayers`/`clearLayers`/
   `destroyLayers` → `std::vector<std::vector<unsigned char>>`. Keep
   `getImageMap()` returning `unsigned char*` — `StreamTexture::update` and the
   loaders depend on the contiguous buffer. This is the largest single change
   in the plan (~700-line file); it removes the `m_ownData` flag hazard and the
   hand-rolled destructor.
3. **`Clipboard` / `Texture`-style raw getters**: `Image* getClipboard()` and
   friends return raw pointers out of `shared_ptr` members. Leave the signature
   (consumers are many) but document the non-owning contract, or switch to
   returning a reference where null is not a valid return.
4. Fix the narrowing warnings unmasked in Phase 4.
5. In-class member initialisers throughout, replacing the constructor-body
   assignment blocks; `= delete` copy ops where the class owns GL/heap state.

**DONE (2026-08-02).** This phase turned up real defects rather than just
style issues.

**`Image` storage → `std::vector<std::vector<unsigned char>>`.** The
`unsigned char**` machinery and its `initLayers`/`clearLayers`/`destroyLayers`
trio are gone, taking three latent bugs with them:

1. **Dangling pointers on a non-positive size.** `setSize()` called
   `clearLayers()`, which deleted every layer buffer but never nulled the
   pointers, then skipped reallocation when `width`/`height` were <= 0 —
   leaving `m_imageMaps[]` and `m_imageMap` pointing at freed memory. The new
   code returns before touching storage.
2. **Latent double-free in `setImageMap()`.** It did `delete [] m_imageMap`,
   but that pointer *aliases* one of the `m_imageMaps[]` layer buffers, which
   the destructor would then free again. Unreachable today (the only caller,
   `Clipboard::copyImage`, passes `ownData = false`) but a live trap.
3. **`m_ratio = width/height` was integer division** assigned to a `double` —
   0 for any image taller than it is wide.

`m_imageMap` survives as the non-owning "current layer" pointer, so all the
per-pixel accessors and `getImageMap()` (which `StreamTexture` depends on for a
contiguous buffer) are untouched. A new private `pointAtLayer()` centralises
the aliasing.

**Two dead accessors removed:** `getData()` (redundant with `getImageMap()`)
and `getRatio()`, whose last caller disappeared with `Rectangle::setTexture` in
Phase 6 — which also disposes of bug 3 rather than leaving a broken function in
place.

**`Image` copy operations are now `= delete`d.** Copying was never safe under
the raw-pointer implementation (guaranteed double-free) but the compiler
generated it anyway; with vector members it would silently *become* valid,
which is a behaviour change. Deleting keeps the original contract explicit.

**`SgiImage`** now stores `std::string`, matching `JpegImage`/`PngImage` —
no more `new char[strlen+1]`/`strcpy`/`delete[]`, and `getFileName()` is
`const`. The class is live (it loads the `.rgb` brush masks), so it stays. All
23 `setFileName(x.c_str())` call sites in `PaintView.cpp` now pass the string
directly.

**`Clipboard`'s raw getters** keep their signatures (many callers) but now
document the non-owning contract, per option 3 of the item above.

**Deliberately not fixed:** `setSize()` resets `m_imageMap` to layer 0 while
leaving `m_currentLayer` untouched, so the two disagree until the next
`setLayer()`. `PaintView` toggles layers constantly, so changing that
post-condition is a behavioural change that does not belong inside a refactor
billed as behaviour-preserving. Commented in place.

Verified: clean rebuild of both configs (112 TUs, 0 errors, 0 warnings) plus a
render check on `thick_beam.fp2` — identical to the Phase 6 baseline. That
exercises the rewritten code end to end: model load drives `setSize()`, the
canvas blit reads `getImageMap()`, and the draw path toggles
`setLayer(0)`/`setLayer(1)`.

### Phase 8 — Style, API polish, and tooling

Lowest priority, highest churn — treat as opt-in, and *do not* start it before
phases 1–7 are merged.

- **Singletons**: replace `SingletonDestroyer<T>` in `common/UiSettings` and
  `forcepad/PlatformInfo` with a function-local `static`; delete
  `SingletonDestroyer.h`.
- **Const-correctness**: `getWidth()`, `getHeight()`, `getRotation()`,
  `getColor()`, `x()`, `y()` etc. are all non-`const`. Adding `const` ripples
  into `common`, so batch it.
- **Out-params → return values**: `getPosition(double&, double&)` →
  `std::pair`/small struct or `Vec2`. Add the new overload, migrate callers,
  then remove the old — do not flip in place.
- **`enum` → `enum class`** for `Rectangle::TRectangleType`, `TLineType`,
  `Clipboard::TPasteMode`, `TCopyImageMode`, `ScreenImage::TRenderMode`. This
  one *is* a source-compatibility break at every call site; weigh it.
- **`Vec3d`** is a 3-component vector in a 2D library, with a `virtual`
  destructor it does not need (making it non-trivially-copyable). Either slim
  it to a plain aggregate `struct` or replace its handful of uses with `Mat4`-
  adjacent helpers.
- Enable warnings on the `ivf2d` target only
  (`target_compile_options(ivf2d PRIVATE /W4)` / `-Wall -Wextra`) once Phase 7
  has cleared the backlog, so regressions surface immediately.
- Run `clang-tidy` with `modernize-*` + `cppcoreguidelines-*` over `ivf2d`
  using the `build-uml/compile_commands.json` the UML script already produces
  (see `CLAUDE.md` — same database, no new tooling needed).
- Re-verify the **wasm** build end-to-end (`scripts/wasm-build.ps1` +
  `wasm-serve.ps1`), and re-run `scripts/check-legacy-gl.sh`.

**DONE (2026-08-02),** except the clang-tidy pass, which was declined.

**`/W4` on `ivf2d` — 13 warnings, all fixed.** The measurement the plan asked
for: 8 x C4244 (narrowing) and 5 x C4100 (unreferenced parameter). Two were
real defects rather than noise:

- **`Arch.cpp` silently truncated arch curve coordinates to whole pixels.**
  `p1.setComponents(m_size[0]/2.0 + x, y)` has only one two-argument overload
  to bind to -- `setComponents(int, int)` -- so both doubles were narrowed.
  Fixed by calling the three-argument `(double, double, double)` overload,
  which is what every other call site in the tree uses. This *changes rendered
  output* (the arch is now drawn at sub-pixel precision) and is the one
  behavioural improvement in the phase.
- **`Image::createAlphaAll(max, min)` ignored both parameters** and had zero
  callers. Deleted rather than silenced -- a function whose signature lies
  about what it does is worse than no function.

The rest: `Clipboard`'s `int` selection members were initialised from `0.0`;
`Base`'s stream stubs and `SgiImage::expandrow`'s vestigial `z` now use unnamed
parameters. `/W4` is now wired into `src/ivf2d/CMakeLists.txt` (`-Wall -Wextra`
off MSVC), scoped to this target only -- the other libraries have not been
through the modernisation.

**Singleton**: `SingletonDestroyer<T>` replaced with a function-local static in
`UiSettings` (the only remaining user after `PlatformInfo` went with the FLTK
port), and the header deleted.

**`Vec3d`**: destructor was `virtual` with nothing deriving from it and no other
virtuals -- a vtable pointer bolted onto three doubles, and enough to make the
type non-trivially-copyable. Now `= default` and non-virtual; the copy
constructor is `= default` too.

**const-correctness**: 31 accessors across 10 classes, declaration and
definition. This turned out to be *additive*, not a ripple: adding `const` to a
member function never breaks a caller, and `ivf2d` has no virtual getters whose
signatures would have to match. Deliberately excluded are the accessors handing
out mutable pointers into internals (`Image::getImageMap`,
`Vec3d::getComponents()`, `Clipboard::getClipboard*`) -- const on those would
let callers mutate through a const object.

**Out-params -> return values: mostly dissolved into dead-code removal.** Of
the getters the plan named, `ScreenImage::getTiles/getRows/getCols/
getRenderMode/getSubImageSize` and `Point::getPosition()` (the raw-pointer
overload) had **zero callers** and were deleted. `Rectangle::getSize` and
`Arch::getSize` had zero callers too, so converting them to return the new
`Vec2d` was free. `Clipboard::getSelection` (2 callers) now returns `Rect2i`.
New header `Vec2d.h` holds both aggregates -- the library is 2D but previously
had only the 3D `Vec3d`.

**`getPosition(double&, double&)` was deliberately left alone.** All 32 call
sites have the shape `expr->getPosition(a, b);` with `a`/`b` already declared
and used separately downstream. Returning a struct there means every call site
grows a `const auto p = ...; a = p.x; b = p.y;` unpack -- strictly worse code
unless the surrounding logic is rewritten to use the aggregate, which is a
manual refactor of drawing code well beyond an API cleanup. The conversion only
pays off where callers can adopt the returned value, which is why the
zero-and-two-caller getters were converted and this one was not.

**`enum` -> `enum class`**: all five (`Rectangle::TRectangleType`/`TLineType`,
`Clipboard::TPasteMode`/`TCopyImageMode`, `ScreenImage::TRenderMode`), 46
enumerator references qualified. **Trap worth recording:** `PaintView` has its
own unrelated `IM_NEW_MODEL`/`IM_PASTE` enumerators that share the `IM_` prefix
with `Clipboard::TCopyImageMode`. A prefix-based rename would have silently
broken them; the conversion matched on exact enumerator names instead.

Also done in this phase (carried over from earlier deferrals):

- **`PaintView.cpp` `USE_QT` collapse** -- the four remaining dead guards
  removed. That in turn made `m_mainFrame` write-only (it was read only in the
  FLTK branch), and `setMainFrame()` had no callers at all, so both went.
- **`Base.h` include trim** -- down from nine standard headers to `<iosfwd>`
  and `<memory>`. The cascade was resolved by scanning every file for std
  facilities used without the providing header rather than iterating one build
  at a time: **44 includes added across 32 files**. One subtlety `<iosfwd>`
  introduces: files that *use* `operator<<`/`>>` need the complete
  `<ostream>`/`<istream>`, not just the forward declarations, which is why
  `Constraint.cpp`, `Force.cpp` and `FemGrid2.cpp` gained those explicitly.

Verified: clean rebuild of both configs, 0 errors, 0 warnings; `ivf2d` clean at
`/W4`; `check-legacy-gl.sh` passing both invariants; render check on
`thick_beam.fp2` matching the Phase 6/7 baseline.

**wasm re-verification (2026-08-02): passed, after two fixes it uncovered.**
Clang warns about things MSVC accepts silently:

- **`FemGrid2::readFromStream`/`saveToStream` overrode `Base`'s virtuals
  without `override`** (24 warnings). The same gap Phase 5 closed for the
  `doGeometry` hooks -- missed there because MSVC does not diagnose it. Now
  marked.
- **Include-case mismatches** across 8 files: `#include "QtPaintView.h"` /
  `"MainWindow.h"` against the on-disk `qtpaintview.h` / `mainwindow.h`.
  Harmless on Windows, a hard failure on any case-sensitive filesystem -- a
  latent portability bug in a project targeting Emscripten. Fixed to match the
  real filenames.

wasm then compiled **0 errors, 0 warnings**, and the app renders in Chrome
(full UI, tool palette, Properties panel, WebGL2 canvas).

**Separately: `scripts/wasm-build.ps1` was broken before any of this work.**
Line 58 passed `-DCMAKE_BUILD_TYPE=$Config` unquoted, so PowerShell handed the
literal string `$Config` to the `qt-cmake.bat` wrapper. The failure surfaced far
from its cause, as `ninja: error: CMakeFiles/rules.ninja:25: expected newline,
got lexing error` on `rule CXX_COMPILER__spdlog_unscanned_$Config`. Fixed by
quoting the argument; the comment there records the symptom so it does not have
to be re-derived.

---

## 4. Effort and risk

| Phase | Files touched | Risk | Notes |
|---|---|---|---|
| 1 Dead files | ~6 | None | Provably uncompiled code |
| 2 RTTI macros | ~38 | None | Zero call sites |
| 3 Pointer typedefs | ~24 | Very low | Consumers untouched (names preserved) |
| 4 `CommonDefs.h` | ~15 + fallout | **Medium** | `windows.h` removal will surface hidden dependencies; warning flood expected |
| 5 `Base` slimming | ~8 | Low | |
| 6 `Texture` removal | ~7 | Low | Verify shape-commit path renders |
| 7 Memory hygiene | ~6 | Medium | `Image.cpp` layer rework is the one real refactor |
| 8 Style/API | ~40 | Medium-high | Ripples into `common`/`paintview`; opt-in |

Phases 1–3 together delete roughly 400 lines and can land in a single sitting.
Phase 4 is the one that changes the codebase's character. Phases 7–8 are
genuine engineering and should be scheduled, not sprinted.

---

## 5. Decisions to confirm

1. **Keep `ivf2d` self-contained** (no dependency on the separate `ivf2` repo)
   — already settled during the rendering migration; this plan assumes it
   holds.
2. **`Base` survives** as a minimal serialisation/polymorphic base rather than
   being dissolved into `common`.
3. **FLTK target is best-effort**, not a verification gate.
4. **Phase 8 is opt-in** — phases 1–7 deliver essentially all the value with a
   fraction of the churn.
