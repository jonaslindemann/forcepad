//
// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2026 Division of Structural Mecahnics, Lund University
//
// Written by Jonas Lindemann
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//

#pragma once

#include <memory>
#include <vector>

#include "Mat4.h"

// Forward-declared so this header carries no Qt dependency and can be included
// by framework-agnostic consumers (e.g. the `common` library). The Qt-backed
// GLProgram lives entirely in Renderer2D.cpp.
class QOpenGLExtraFunctions;

namespace ivf2d {

class GLProgram;

/**
 * Immediate-mode-style batching renderer built on modern OpenGL.
 *
 * This is the drop-in replacement for the fixed-function drawing scattered
 * through the ForcePAD renderers. It reproduces the shape of the old
 * glBegin/glColor/glVertex/glEnd code so that each block can be ported almost
 * mechanically:
 *
 *   old:                                new:
 *   glBegin(GL_QUADS);                  auto &r = Renderer2D::instance();
 *   glColor3fv(c);                      r.beginQuads();
 *   glVertex2i(x, y);                   r.color(cr, cg, cb);
 *   ...                                 r.vertex(x, y);
 *   glEnd();                            ...
 *                                       r.end();
 *
 * The fixed-function matrix stack is replaced by setOrtho()/pushTransform()/
 * translate()/rotateZ()/scale(). GL_QUADS (absent from core/ES) is emulated by
 * expanding each group of four vertices into two triangles at flush time.
 *
 * Implemented as a per-context singleton: there is a single GL context in the
 * ForcePAD widget, and all GL objects (program, VBO, VAO) live in it. Resources
 * are created lazily on first use with the current context bound.
 */
class Renderer2D {
public:
    enum Primitive {
        Triangles,
        TriangleStrip,
        TriangleFan,
        Lines,
        LineStrip,
        LineLoop,
        Points,
        Quads // emulated: expanded to Triangles on flush
    };

    static Renderer2D &instance();

    /** Releases GL resources. Call with the owning context current. */
    void destroy();

    // --- projection / transform (replaces the fixed-function matrix stack) ---

    /** Sets the projection, equivalent to gluOrtho2D(l, r, b, t). */
    void setOrtho(float l, float r, float b, float t);
    void setProjection(const Mat4 &projection);

    void loadIdentity();
    void pushTransform();
    void popTransform();
    void translate(float x, float y);
    void rotateZ(float degrees);
    void scale(float sx, float sy);

    /** Resets the transform stack to a single identity (call once per frame). */
    void resetTransform();

    // --- GL state helpers so callers need no direct GL/Qt dependency ---

    void setViewport(int x, int y, int w, int h);
    void setScissor(int x, int y, int w, int h);
    void setScissorEnabled(bool enable);
    void clear(float r, float g, float b, float a);

    // --- immediate-mode-style batching ---

    void begin(Primitive primitive);
    void beginQuads() { begin(Quads); }
    void beginTriangles() { begin(Triangles); }
    void beginTriangleFan() { begin(TriangleFan); }
    void beginLines(float width = 1.0f);
    void beginLineStrip(float width = 1.0f);
    void beginLineLoop(float width = 1.0f);

    /**
     * Dashed GL_LINES batch, replacing glLineStipple. The dash distance is
     * computed automatically per segment (pixels on / off = periodPixels/2),
     * so callers just emit vertex pairs as with beginLines().
     */
    void beginDashedLines(float width, float periodPixels);

    void color(float r, float g, float b, float a = 1.0f);
    void color3fv(const float *rgb) { color(rgb[0], rgb[1], rgb[2], 1.0f); }
    void texCoord(float u, float v);
    void vertex(float x, float y);

    /** Flushes the accumulated primitive as a single draw call. */
    void end();

    // --- textured quad (used by StreamTexture) ---

    void drawTexturedQuad(unsigned int texture,
                          float x, float y, float w, float h,
                          float u0 = 0.0f, float v0 = 0.0f,
                          float u1 = 1.0f, float v1 = 1.0f,
                          float tintR = 1.0f, float tintG = 1.0f,
                          float tintB = 1.0f, float tintA = 1.0f);

    // --- state helpers (replace glEnable(GL_BLEND) etc.) ---

    void setBlend(bool enable);

    /**
     * Forces fragment alpha to 1.0 for subsequent draws. Reproduces the
     * glPixelTransfer(GL_ALPHA_SCALE=0, GL_ALPHA_BIAS=1) trick the paint canvas
     * relies on, where the image alpha channel encodes layer markers rather
     * than opacity and must not bleed into the (alpha-composited) FBO.
     */
    void setForceOpaque(bool enable);

private:
    Renderer2D();
    ~Renderer2D();
    Renderer2D(const Renderer2D &) = delete;
    Renderer2D &operator=(const Renderer2D &) = delete;

    static QOpenGLExtraFunctions *gl();
    bool ensureInit();
    const Mat4 &currentTransform() const { return m_transformStack.back(); }

    std::unique_ptr<GLProgram> m_program;
    unsigned int m_vbo{0};
    unsigned int m_vao{0};
    bool m_initialized{false};

    Mat4 m_projection;
    std::vector<Mat4> m_transformStack{Mat4()};
    int m_viewportW{0};
    int m_viewportH{0};

    // Accumulation state
    Primitive m_primitive{Triangles};
    bool m_inBatch{false};
    bool m_forceOpaque{false};
    bool m_dashed{false};
    float m_dashPeriod{16.0f};
    float m_lineWidth{1.0f};
    float m_color[4]{1.0f, 1.0f, 1.0f, 1.0f};
    float m_tex[2]{0.0f, 0.0f};
    std::vector<float> m_vertices; // interleaved: x, y, r, g, b, a, u, v
};

} // namespace ivf2d
