# Rendering Migration Plan — Fixed-function → Modern shader-based GL

**Goal:** Replace all legacy/fixed-function OpenGL in the ForcePAD rendering
path with a small modern-GL abstraction (VBO + shaders, GLES-compatible), so
that the *desktop* `qtforcepad` build keeps working while the code becomes
portable to **WebGL 2 (OpenGL ES 3.0)** for the future Qt-for-WebAssembly port.

This is **Step 1** of the web-port effort. It touches only rendering; the FEM
model, solver, and UI are untouched. It is fully testable on the desktop build
before any WebAssembly work begins.

---

## 1. Why this is the whole game

WebGL / OpenGL ES has **no fixed-function pipeline**: no `glBegin/glEnd`, no
matrix stack (`glMatrixMode`, `gluOrtho2D`, `glPushMatrix`), no `glDrawPixels`,
no `glLineStipple`, no `glLogicOp`, no `glPolygonMode`, no `glPushAttrib`, no
`glTexEnv`. The current renderer uses all of these.

Current context request that makes the legacy code work today:

- `src/qtforcepad/main.cpp:44` — `fmt.setVersion(2, 1)`
- `src/qtforcepad/main.cpp:45` — `QSurfaceFormat::CompatibilityProfile`

The migration flips this to a **3.3 Core** profile on desktop. In a core
profile every remaining fixed-function call fails loudly (GL error / no draw),
which is exactly the tripwire we want: when the app renders correctly under 3.3
Core, it is structurally ready for GLES 3.0 / WebGL 2.

---

## 2. Rendering inventory (what must change)

Two drawing mechanisms exist:

1. **CPU-raster blits** — `ImageGrid`/`ScreenImage` hold a CPU pixel buffer that
   is pushed to the framebuffer with `glDrawPixels`. This covers the drawing
   canvas, the clipboard preview, and the FEM stress/displacement field image.
2. **Vector overlays** — mesh, forces, constraints, shapes, ruler, selection
   box, drawn in immediate mode.

### Files with GL calls

| File | Role | GL constructs to replace | Weight |
|---|---|---|---|
| `src/paintview/PaintView.cpp` | main `onDraw`/`onInitContext`, readback | matrix stack, `gluOrtho2D`, background `glBegin` quads, `glDrawPixels`+`glLogicOp` paste, `glPushAttrib`, `glScissor` (keep), `glReadPixels` (keep) | High |
| `src/common/FemGrid2.cpp` | mesh + stress field + forces/constraints | ~55 calls: `glBegin`, `glLineWidth`, `glPolygonMode`, `glTexEnvf`, `glBindTexture`, `glPushAttrib` | **Critical path** |
| `src/ivf2d/ScreenImage.cpp` | CPU-raster blitter | `glDrawPixels`, `glPixelStorei`, `glRasterPos`, `glReadPixels` | High |
| `src/ivf2d/Rectangle.cpp` | selection marquee | `glLineStipple`, `glPushAttrib`, `glBegin` | Medium |
| `src/ivf2d/Shape.cpp`, `DrawableBase.h` | base transform/render | `glPushMatrix/glTranslated/glRotatef` | Medium |
| `src/ivf2d/Arch.cpp`, `Ellipse.cpp`, `Line.cpp` | shape previews | `glBegin/glEnd` | Low each |
| `src/ivf2d/Texture.cpp` | texture object | already modern (`glGenTextures`/`glBindTexture`) | Trivial |
| `src/common/Force.cpp`, `Constraint.cpp`, `ReactionForce.cpp`, `CGIndicator.cpp`, `Ruler.cpp`, `ImageGrid2.cpp` | overlays / arrows | `glBegin/glEnd`, matrix | Medium (total) |
| `src/ivf2d/Clipboard.cpp`, `Arrow.cpp`, `Point.cpp` | overlays | `glBegin/glEnd` | Low |

> Note: `ivf2d` (in-tree, legacy immediate-mode 2D lib) is **not** the modern
> `ivf2` repo. **Decided:** modernize `ivf2d` in place and keep it
> **self-contained** — borrow shader/patterns from `ivf2` where useful, but take
> no dependency on it. This keeps the port isolated and low-risk.

---

## 3. The abstraction layer (new code)

Introduce a minimal, framework-agnostic 2D renderer in `src/ivf2d/` so both
desktop and (later) wasm share one path. Keep the API close to the old
immediate-mode shape so each `render()` ports almost 1:1.

### 3.1 `GLShader` / `GLProgram`
Thin compile/link/uniform wrapper. Shader source carries **no version line**;
the version + precision header is prepended at load time:

- Desktop 3.3 Core → `#version 330 core\n`
- WebGL 2 / GLES 3.0 → `#version 300 es\nprecision highp float;\n`

The GLSL body is identical across both. (Pattern already proven in the `ivf2`
repo's `program.cpp`.)

### 3.2 `Renderer2D` — the immediate-mode shim
A batching helper that makes porting mechanical:

```cpp
r.setProjectionOrtho(l, r_, b, t);   // replaces gluOrtho2D
r.pushTransform(); r.translate(x,y); // replaces glPushMatrix/glTranslated
r.beginQuads();                      // replaces glBegin(GL_QUADS)
r.color(rr,gg,bb,aa); r.vertex(x,y); // replaces glColor/glVertex2i
r.end();                             // flushes one VBO draw call
r.beginLines(width, dashed);         // replaces glLineWidth + glLineStipple
```

Internally: one dynamic VBO (position + RGBA color), one shader
(position/color, MVP uniform). `beginXxx/vertex/end` accumulate into a
client-side vertex array and issue a single `glDrawArrays` per `end()`.
This lets each `glBegin…glEnd` block become a near-identical shim block.

A tiny `Mat4` (ortho + translate/rotate/scale) replaces the fixed-function
matrix stack. `pushTransform/popTransform` maintain a `std::vector<Mat4>`.

### 3.3 `StreamTexture` — the `glDrawPixels` replacement
Wraps a GL texture updated from a CPU `ImageGrid` buffer:

- `upload(imageMap, w, h, format)` → `glTexSubImage2D` (allocate lazily / on
  resize with `glTexImage2D`).
- `draw(x, y, w, h, zoom)` → textured quad via `Renderer2D`.
- Supports the `RM_SUBIMAGE` / `RM_TILED` sub-region cases by adjusting UVs
  (replacing `GL_UNPACK_ROW_LENGTH`/`SKIP_PIXELS`/`SKIP_ROWS`).

This single class replaces every `glDrawPixels` site (ScreenImage, drawing
canvas, clipboard, FemGrid2 stress image).

---

## 4. Tricky cases (design decisions needed)

1. **Clipboard paste preview** (`PaintView.cpp` ~1203): uses
   `glLogicOp(GL_AND)` + `GL_COLOR_LOGIC_OP` for an XOR/AND ghost. No logic ops
   in GLES. → **Decided:** replace with a plain **alpha-blended ghost**
   (`GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA`). Exact AND semantics are not
   essential for this educational tool; confirm visually during Phase 4.
2. **Selection marquee stipple** (`Rectangle.cpp`): `glLineStipple`. → Dashed
   lines via a fragment shader using a `varying` arc-length + `discard`, or a
   1-D dash texture. Small self-contained shader.
3. **Wireframe** (`FemGrid2.cpp`): `glPolygonMode(GL_LINE)`. → Emit explicit
   line-primitive geometry for the mesh edges instead of relying on polygon
   mode.
4. **Line width > 1** (`glLineWidth`): not guaranteed in core/WebGL. → For thin
   grid lines this is acceptable; for thick force arrows, build them as
   triangles (the arrow geometry is already explicit).
5. **`glReadPixels`** (`PaintView.cpp` ~2540, `ScreenImage.cpp` ~227): exists in
   WebGL but only `GL_RGBA/UNSIGNED_BYTE` is guaranteed. → Keep, but pin format
   to RGBA8 and flip rows in software where needed.
6. **`glPushAttrib/glPopAttrib`**: no equivalent. → Replace each with explicit
   save/restore of the specific state (blend enable, blend func) around the
   block, ideally centralized in `Renderer2D` state setters.

---

## 5. Phasing (each phase leaves desktop build green)

**Phase 0 — Scaffolding. [DONE]** Added `Mat4`, `GLProgram`, `Renderer2D`,
`StreamTexture` to `ivf2d`, resolving GL entry points through Qt's
`QOpenGLExtraFunctions` (works on desktop and WebGL 2 with no GLAD/GLEW). A
smoke test (env var `FORCEPAD_R2D_SMOKETEST`) draws a gradient quad, a streamed
checkerboard texture, and batched lines through the new path; run it to see the
modern renderer in isolation. Notes from implementation:

- `main.cpp` now requests **3.3 Compatibility** on Windows/Linux (a superset of
  2.1 that also enables shaders/VBO/VAO), and stays on **2.1** on macOS (no
  >2.1 compatibility profile exists there — macOS jumps straight to Core at the
  Phase 5 flip). Verified: NVIDIA returns a 4.6 Compatibility context, existing
  immediate-mode rendering unaffected, `Renderer2D` initializes and shaders
  compile with the `#version 330 core` header.
- **Gotcha:** Qt6 is linked to `ivf2d` as **PRIVATE**, not PUBLIC. A PUBLIC
  link leaks Qt's `UNICODE`/`_UNICODE` compile definitions into `common`, which
  flips the Win32 TCHAR macros and breaks `SystemInfo.cpp`'s ANSI registry
  calls. Keep future Qt links to the shared libs PRIVATE (or fix the ANSI
  assumptions first).

**Phase 1 — Blitter. [DONE]** Converted `ivf2d::ScreenImage` to `StreamTexture`
+ `Renderer2D`, replacing its `glDrawPixels`/`glRasterPos`/`glPixelZoom` blit
(all three render modes) and routing its `glReadPixels` capture through
`QOpenGLExtraFunctions`. The class is now free of system-GL headers and legacy
raster/matrix state (its `doBeginTransform`/`doEndTransform` are no-ops; it
positions the quad itself). Notes:

- Added `Renderer2D::setForceOpaque()` (fragment alpha forced to 1) to reproduce
  the `glPixelTransfer(GL_ALPHA_SCALE=0, GL_ALPHA_BIAS=1)` trick in
  `QtPaintView::paintGL` — the canvas image's alpha encodes layer markers, not
  opacity, and must not bleed into the alpha-composited FBO.
- `PaintView::onDraw` sets `Renderer2D::instance().setOrtho(0,w,0,h)` before the
  screen-image blit (guarded `#ifdef USE_QT`, matching the file's convention) so
  the modern projection tracks the still-fixed-function ortho during the
  transition. The `glPixelZoom`/`glPixelTransfer` calls remain for the
  not-yet-migrated `glDrawPixels` sites (paste, force/constraint overlays).
- Verified with sample models and an empty canvas (screenshots): canvas fills
  solid, correct position/orientation, **no gray bleed**.
- **Observation (not a regression):** a faint diagonal line is drawn over the
  canvas by `FemGrid2::render()` (the only overlay active at startup, edit mode
  `EM_DIRECT_BRUSH`). It is unmodified immediate-mode code, pre-existing, and
  will be resolved in Phase 3. Confirmed by drawing the canvas as a flat quad —
  the line renders *on top* of it, so it is an overlay, not a tessellation seam.
- **FLTK note:** because `ivf2d` now depends on Qt (via `Renderer2D`), the
  legacy `BUILD_FLTK_APP` target would need Qt to link. It is off by default and
  effectively deprecated by this Qt-centric migration.

**Phase 2 — Overlays. [DONE]** Converted the vector renderers to `Renderer2D`:
`ivf2d` shapes (`Arch/Ellipse/Line/Rectangle`), `Shape` transforms, `Color`,
and the `common` overlays (`Force/Constraint/ReactionForce/CGIndicator/Ruler`;
`Arrow/Point` have no geometry). Mechanisms added:

- **`Renderer2D::syncFromGL()`** — transitional bridge reading the fixed-function
  `GL_PROJECTION`/`GL_MODELVIEW` so converted overlays align with the still
  immediate-mode code (`PaintView`, and `FemGrid2` until Phase 3) that positions
  them. Depth-tracked via `pushModelTransform`/`popModelTransform` so nested
  renders (Arch→Ellipse, Constraint→ReactionForce) compose correctly. Removed at
  the Phase 5 core flip.
- **`Color::doMaterial()`** and direct `glColor3f` now set `Renderer2D`'s current
  color; `Shape::doBeginTransform/doEndTransform` drive the `Renderer2D`
  transform stack (the old raster/matrix branches are gone).
- **Dashed lines** (`beginDashedLines`) replace `glLineStipple` for the selection
  marquee, ruler measure-line, and hinge constraint — a fragment-shader discard
  keyed on auto-computed per-segment distance. `GL_POLYGON` → `TriangleFan`.
- **`Renderer2D.h` is now Qt-free** (GLProgram pimpl'd behind a forward-declared
  `unique_ptr`, `QOpenGLExtraFunctions` forward-declared) so the framework-
  agnostic `common` library can include it without pulling in Qt (which would
  re-trigger the `SystemInfo.cpp` UNICODE break). `common` links `ivf2d` for the
  symbols; no Qt dependency added to `common`.
- Ambient GL blend state set by `PaintView` still applies to `Renderer2D` draws
  (blending is a fixed-function output stage, independent of the shader), so
  translucent overlays keep working during the transition.
- **Critical transitional rule:** `Renderer2D` must call `glUseProgram(0)` (and
  unbind textures) after every draw. In a compatibility context a left-bound
  shader program also processes `glDrawPixels`/immediate-mode fragments, which
  rendered the canvas black in the force/BC edit modes (they blit `m_drawing`
  via `glDrawPixels`). Restoring the fixed-function pipeline after each batch
  fixes it. This restore goes away at the Phase 5 core flip.
- Verified: `thick_beam.fp2` renders force arrows and constraint symbols
  (filled discs + line loops) correctly positioned — proving `syncFromGL` works
  when converted overlays are invoked from immediate-mode `FemGrid2`.

**Phase 3 — FemGrid2. [DONE]** Converted the whole `FemGrid2::doGeometry`
dispatch and every `draw*` helper to `Renderer2D`, plus `ImageGrid2::doGeometry`.
`FemGrid2.cpp`/`ImageGrid2.cpp` now include only `Renderer2D.h` (no GL/Qt).
Details:

- **Grid fills** (`drawGrid`/`drawDensity`/`drawUndeformedGrid`/`drawStructure`)
  and **Mises stress** (`drawMisesStress`): `glBegin(GL_QUADS)`+`glColor4f` →
  `beginQuads`/`color`/`vertex`.
- **Smooth Mises colormap** (`drawMisesStressSmooth`): the 1D colormap texture
  (`GL_TEXTURE_1D` + `glTexCoord1f`, absent from core/WebGL) was **dropped** in
  favour of the CPU `m_colorMap->getColor()` per node (the code already had this
  path behind `if(false)`). Vertex-color interpolation replaces texture
  sampling; visually equivalent for the smooth colormap. Verified: the stress
  field renders with the correct smooth blue→red gradient and hotspots.
- **Principal stress** (`drawStress`/`drawStressArrow`): `GL_LINES` → batched
  `beginLines`; per-segment colors preserved.
- **`drawForces`/`drawConstraints`**: dropped `glPushAttrib`/`glEnable(BLEND)`/
  `glEnable(LINE_SMOOTH/POLYGON_SMOOTH)`/`glPopAttrib`; the render loops call the
  already-converted `Force`/`Constraint`. Blend is relied on ambiently (PaintView
  enables it for the whole overlay pass).
- **Wireframe** (`drawGridPoints`, `glPolygonMode(GL_LINE)`) → explicit
  `beginLineLoop` per element. `drawDebugPoints` → batched lines.
- **`GLubyte` → `unsigned char`** in the two files (colormap/pixel CPU arrays)
  now that the GL headers are gone.
- **The pre-existing canvas diagonal is resolved** — it was `FemGrid2`'s
  immediate-mode geometry (rendering oddly through the transitional bound-shader
  state). With `FemGrid2` on `Renderer2D`, the Sketch view is clean.
- Verified end-to-end on `thick_beam.fp2`: Sketch (clean, no diagonal), Physics
  (forces/constraints/reactions), and Action — both the principal-stress line
  field and the smooth Mises colormap field render correctly.

**Phase 4 — PaintView. [DONE]** Converted `onDraw`, `onInitContext`, and
`onClear` fully to `Renderer2D`. This also folded in the projection handoff that
was originally slated for Phase 5:

- Background gradient + frame-shadow quads → `beginQuads`; projection via
  `setOrtho`/`setViewport`; scissor via `setScissorEnabled`/`setScissor`; clear
  via `clear()`; blend via `setBlend()`. Added those state helpers +
  `resetTransform()` to `Renderer2D`.
- **`syncFromGL()` removed.** Projection is now set explicitly by `PaintView`
  before each group of draws (`setOrtho`/`resetTransform`), and `Shape`
  transforms just push onto the stack. No code reads `GL_PROJECTION_MATRIX`
  anymore — the fixed-function matrix stack is fully gone from the render path.
- **Paste preview** → alpha-blended clipboard ghost via a `StreamTexture`
  (drops `glLogicOp(GL_AND)`); the force/BC-mode `glDrawPixels` overlay →
  `m_screenImage->render()`. `glPixelZoom` gone.
- **`glPixelTransfer` alpha-forcing removed** from `QtPaintView` (paintGL and the
  undo-capture path); `Renderer2D::setForceOpaque` handles the canvas alpha now.
- `StreamTexture.h` made Qt-free (forward-declared, `ST_RGBA/ST_RGB` constants)
  so the shared `PaintView.cpp` can include it without pulling Qt into the FLTK
  compile.
- **Remaining raw GL in `PaintView.cpp`:** only `glGetString` (version log) and
  `glReadPixels`/`glPixelStorei` (framebuffer readback for undo/save) — all
  core-profile-safe; left for Phase 6 (or wrap in Renderer2D helpers then).
- Verified on `thick_beam.fp2`: Sketch (gradient background, canvas, symbols),
  Physics + Force tool (no black canvas), Action (smooth Mises colormap stress
  field) all correct under the fully explicit-projection path.
- **Line thickness:** `glLineWidth > 1` is clamped to 1 in a core profile /
  WebGL, so after the Core flip the force/BC stroke thickness had no effect.
  `Renderer2D` now expands thick lines (`GL_LINES`, `GL_LINE_STRIP`,
  `GL_LINE_LOOP` with width > 1) into quad geometry, so force arrows, constraint
  crosses/bars *and* the constraint rings honour the line-thickness setting
  again. Width is converted from device px to local units via the projection +
  viewport scale (DPR- and zoom-correct); strips/loops use square caps so ring
  segments join without gaps.
- **Antialiasing:** the migration dropped `glEnable(GL_LINE_SMOOTH)`/
  `GL_POLYGON_SMOOTH` (not in core/WebGL), which made overlays look jagged.
  Restored via **MSAA** — `fmt.setSamples(4)` on the `QSurfaceFormat` in
  `main.cpp`. `QOpenGLWidget` creates + resolves a multisampled FBO; this is the
  core-profile / WebGL (`antialias: true`) compatible approach and smooths all
  primitives (lines, arrows, BC circles, polygon edges).
- **MSAA gotcha 1 (black surround):** the multisample-FBO resolve blit
  (`glBlitFramebuffer`, run by Qt after `paintGL`) honours `GL_SCISSOR_TEST`.
  `onDraw` leaves scissor enabled (clipped to the drawing area), so the resolve
  copied only that area and the surround rendered **black**. Fixed by disabling
  the scissor test at the end of `QtPaintView::paintGL`
  (`Renderer2D::setScissorEnabled(false)`) so the resolve covers the whole widget.
- **MSAA gotcha 2 (shapes not committed):** rectangle/line/ellipse/arch are
  committed on mouse-release by rendering the shape and reading it back with
  `glReadPixels` (`onRelease` -> `ScreenImage::update`). `glReadPixels` is invalid
  on a *multisample* FBO, so with MSAA the shape rendered as a live preview but
  vanished on release (the brush was unaffected - it rasterizes into the drawing
  on the CPU). Fixed by rendering the release-time capture pass into a dedicated
  single-sample FBO (`QtPaintView::ensureCaptureFramebuffer`) and reading back
  from that (AA is irrelevant for the captured pixel raster).

**Phase 5 — Flip the profile. [DONE]** `main.cpp` now requests **3.3 Core** on
all platforms (was 3.3 Compatibility; macOS was 2.1). Because Phases 1-4 had
already removed every fixed-function/immediate-mode call from the *runtime*
path, the flip was clean:

- Verified the context reports **OpenGL 3.3.0** (was 4.6 Compatibility),
  `Renderer2D` shaders compile with the `#version 330 core` header (already the
  default from `GLProgram::glslHeader`), no GL errors, empty stderr.
- Verified end-to-end on Core: Sketch (gradient background + canvas + symbols),
  Action (smooth Mises colormap stress + forces/constraints/reactions), and
  shape commit (drew a rectangle and an ellipse — both persist via the
  single-sample capture FBO + `glReadPixels`). All antialiased via MSAA.
- The GLES shader-header path already exists (`GLProgram::glslHeader` emits
  `#version 300 es` when `QOpenGLContext::isOpenGLES()`), so the shaders are
  wasm-ready without a separate code path.
- **Dead legacy code remains but is never executed:** the v1 `FemGrid.cpp` /
  `ImageGrid.cpp` and `ivf2d/Texture.cpp` still contain immediate-mode calls,
  but nothing calls them (v2 classes are used; `Shape` no longer applies
  textures). They compile (they keep their own GL includes) and are inert at
  runtime. Removing/guarding them is Phase 6.

**Phase 6 — Cleanup. [DONE]** Locked in the migration and removed the last dead
fixed-function code:

- **Grep gate**: `scripts/check-legacy-gl.sh` scans the modern render path
  (`ivf2d`, `common`, `paintview`, `qtforcepad`) and fails if any forbidden
  legacy/immediate-mode call reappears (`glBegin`, `glVertex*`, `glColor[34]*`,
  `glMatrixMode`, `gluOrtho2D`, `glPushMatrix`, `glPushAttrib`, `glDrawPixels`,
  `glRasterPos*`, `glPixelZoom`, `glPixelTransfer*`, `glLineStipple`,
  `glPolygonMode`, `glTexEnv*`, `glTranslate*`/`glRotate*`/`glScale*`,
  `glEnableClientState`, `glLogicOp`). Comment-aware (strips `//` and skips block
  comment bodies). Core-safe reads (`glReadPixels`/`glGetString`/`glPixelStorei`)
  are allowed. Wired as the opt-in CMake target `check_legacy_gl` (for CI).
- **Dead code removed/excluded**: gutted the never-called fixed-function bodies in
  `ivf2d/Texture.cpp` (`glMatrixMode`/`glTexEnv`/`glRotated`/`glScaled`); dropped
  the commented-out immediate-mode blocks in `Arch.cpp`/`Rectangle.cpp`; excluded
  the v1 `FemGrid.cpp`/`ImageGrid.cpp` (FLTK-only) from the Qt `common` build;
  removed the now-unused `<GL/glu.h>` include from `PaintView.cpp` (only core-safe
  reads remain there).
- Gate passes; rebuilt and re-verified on Core (OpenGL 3.3.0, renders correctly).

---

## Status: rendering migration COMPLETE

All six phases are done. ForcePAD's Qt build renders entirely through the modern
`Renderer2D`/`StreamTexture` shader pipeline on a **3.3 Core** context - the same
feature level as WebGL 2 / OpenGL ES 3.0 - with MSAA antialiasing and
DPR/zoom-correct thick lines. The shaders already emit a `#version 300 es` header
under a GLES context, so they are WebAssembly-ready. **Step 1 of the web port is
finished; the next effort is standing up the Qt-for-WebAssembly toolchain/build.**

---

## 6. Shader dialect strategy

- One set of GLSL source strings, no `#version` line embedded.
- Runtime prepends the correct header (`330 core` desktop / `300 es` +
  `precision` for WebGL 2).
- Keep shaders trivial: (a) flat color from vertex attribute, (b) textured quad
  with optional tint/alpha, (c) dashed line. Three programs total.

---

## 7. Verification

- **Visual regression:** capture reference PNGs of representative models
  (drawing, forces, constraints, solved stress, displacement, wireframe, paste
  preview, selection marquee, ruler, zoom-results) *before* migration via the
  existing `glReadPixels` save path; diff after each phase.
- **Profile tripwire:** Phase 5's flip to 3.3 Core is the structural gate.
- **grep gate:** CI/pre-commit check that the forbidden constructs listed in §5
  Phase 6 no longer appear in `src/ivf2d`, `src/common`, `src/paintview`.
- No automated tests exist in the repo, so verification is visual + the profile
  gate.

---

## 8. Effort & risk

| Phase | Rough size | Risk |
|---|---|---|
| 0 Scaffolding | S | Low |
| 1 Blitter | M | Low–Med (UV/sub-image math) |
| 2 Overlays | M | Low |
| 3 FemGrid2 | L | **Med–High** (biggest single file, stress field + wireframe) |
| 4 PaintView | M | Med (paste logic-op decision) |
| 5 Profile flip | M | Med (flushing out stragglers) |
| 6 Cleanup | S | Low |

Biggest risks: (1) `FemGrid2` stress-field rendering fidelity; (2) the paste
preview logic-op semantics; (3) DPI/`devicePixelRatio` interaction with the new
projection (currently handled via `glViewport` in physical px + ortho in logical
px — must be preserved in `Renderer2D`).

---

## 9. Decisions (settled)

1. **Paste preview:** use a plain alpha-blended ghost (drop the `GL_AND`
   logic-op). See §4.1.
2. **ivf2d scope:** modernize in place, self-contained; no dependency on the
   `ivf2` repo. See §2 note.
3. **Desktop GL target:** **3.3 Core** — matches `ivf2` and maps cleanly to
   WebGL 2 / GLES 3.0; used as the Phase 5 tripwire.
