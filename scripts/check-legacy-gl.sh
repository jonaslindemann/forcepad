#!/usr/bin/env bash
#
# Rendering-migration guard: fail if legacy / fixed-function OpenGL reappears in
# the modern (Qt6 + WebGL-targeted) render path. The migration replaced all of
# these with the Renderer2D / StreamTexture shader pipeline; none of them exist
# in an OpenGL core profile or in WebGL / OpenGL ES, so their return would break
# the eventual Qt-for-WebAssembly build.
#
# Run from anywhere:  bash scripts/check-legacy-gl.sh
# Exit code 0 = clean, 1 = legacy GL found.
#
# Note: core-safe reads (glReadPixels / glGetString / glPixelStorei) are NOT
# forbidden - they work identically in core / WebGL.

set -uo pipefail
cd "$(dirname "$0")/.."

# Forbidden call patterns (token immediately followed by '('). Split into groups:
#  - plain fixed-function calls
#  - the "typed" immediate-mode calls that take a digit + type suffix
#    (glVertex2d, glColor3f, glRasterPos2i, glTexCoord1f, ...); requiring the
#    digit avoids matching core calls like glVertexAttribPointer / glColorMask
#  - the matrix / texture-env calls with a letter suffix (glTranslated,
#    glRotatef, glScaled, glTexEnvi, glPixelTransferf)
#  - GLU
PATTERN='gl(Begin|End|MatrixMode|LoadIdentity|PushMatrix|PopMatrix|PushAttrib|PopAttrib|DrawPixels|PixelZoom|LineStipple|PolygonMode|EnableClientState|LogicOp)[[:space:]]*\('
PATTERN="$PATTERN"'|gl(Vertex|Color|Normal|RasterPos|TexCoord|MultiTexCoord)[0-9][a-zA-Z]*[[:space:]]*\('
PATTERN="$PATTERN"'|gl(Translate|Rotate|Scale|TexEnv|PixelTransfer)[a-z]+[[:space:]]*\('
PATTERN="$PATTERN"'|glu(Ortho2D|Perspective|LookAt|Project|Build[A-Za-z]*)[[:space:]]*\('

# Directories that make up the modern render path.
DIRS="src/ivf2d src/common src/paintview src/qtforcepad"

fail=0
while IFS= read -r file; do
    # Files intentionally out of scope: the v1 FemGrid/ImageGrid classes are
    # compiled only into the legacy FLTK app, not the Qt/WebGL build.
    case "$file" in
        */common/FemGrid.cpp|*/common/FemGrid.h|*/common/ImageGrid.cpp|*/common/ImageGrid.h)
            continue ;;
    esac

    # Strip // line comments, then drop block-comment body / opener lines
    # (leading '*' or '/') so documentation that references the old calls does
    # not trip the guard. Line numbers stay aligned with the original file.
    hits=$(sed 's://.*$::' "$file" \
        | grep -nE "$PATTERN" \
        | grep -vE '^[0-9]+:[[:space:]]*[*/]')

    if [ -n "$hits" ]; then
        echo "Legacy OpenGL in $file:"
        echo "$hits" | sed 's/^/    /'
        fail=1
    fi
done < <(find $DIRS -type f \( -name '*.cpp' -o -name '*.h' \) | sort)

if [ "$fail" -ne 0 ]; then
    echo ""
    echo "ERROR: legacy / fixed-function OpenGL found in the migrated render path."
    echo "Use Renderer2D / StreamTexture instead (see docs/rendering-migration-plan.md)."
    exit 1
fi

echo "OK: no legacy OpenGL in the migrated render path."
