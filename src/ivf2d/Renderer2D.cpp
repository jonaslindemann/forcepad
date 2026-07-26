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

#include "Renderer2D.h"

#include <cmath>

#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>

#include "FPLog.h"
#include "GLProgram.h"

namespace ivf2d {

// Vertex layout: position (2) + color (4) + texcoord (2) = 8 floats.
static constexpr int kFloatsPerVertex = 8;

// Vertex shader source (no #version line; GLProgram prepends the header).
static const char *kVertexShader = R"GLSL(
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTex;

uniform mat4 uMVP;

out vec4 vColor;
out vec2 vTex;

void main()
{
    vColor = aColor;
    vTex = aTex;
    gl_Position = uMVP * vec4(aPos, 0.0, 1.0);
}
)GLSL";

static const char *kFragmentShader = R"GLSL(
in vec4 vColor;
in vec2 vTex;

uniform int uUseTexture;
uniform int uForceOpaque;
uniform int uDashed;
uniform float uDashPeriod;
uniform sampler2D uTex;

out vec4 fragColor;

void main()
{
    // For dashed lines vTex.x carries the along-segment distance in pixels.
    if (uDashed == 1 && fract(vTex.x / uDashPeriod) >= 0.5)
        discard;

    vec4 c = vColor;
    if (uUseTexture == 1)
        c = c * texture(uTex, vTex);
    if (uForceOpaque == 1)
        c.a = 1.0;
    fragColor = c;
}
)GLSL";

Renderer2D::Renderer2D() = default;
Renderer2D::~Renderer2D() = default;

Renderer2D &Renderer2D::instance()
{
    static Renderer2D s_instance;
    return s_instance;
}

QOpenGLExtraFunctions *Renderer2D::gl()
{
    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    return ctx ? ctx->extraFunctions() : nullptr;
}

bool Renderer2D::ensureInit()
{
    if (m_initialized)
        return true;

    QOpenGLExtraFunctions *f = gl();
    if (f == nullptr)
    {
        fp_error("Renderer2D", "ensureInit() called with no current GL context");
        return false;
    }

    if (!m_program)
        m_program = std::make_unique<GLProgram>();

    if (!m_program->compile(kVertexShader, kFragmentShader))
        return false;

    f->glGenVertexArrays(1, &m_vao);
    f->glGenBuffers(1, &m_vbo);

    f->glBindVertexArray(m_vao);
    f->glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    const int stride = kFloatsPerVertex * sizeof(float);
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void *)0);
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void *)(2 * sizeof(float)));
    f->glEnableVertexAttribArray(2);
    f->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void *)(6 * sizeof(float)));

    f->glBindVertexArray(0);
    f->glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_initialized = true;
    fp_debug("Renderer2D", "Initialized (VAO={}, VBO={})", m_vao, m_vbo);
    return true;
}

void Renderer2D::destroy()
{
    QOpenGLExtraFunctions *f = gl();
    if (f)
    {
        if (m_vbo)
            f->glDeleteBuffers(1, &m_vbo);
        if (m_vao)
            f->glDeleteVertexArrays(1, &m_vao);
    }
    if (m_program)
        m_program->destroy();
    m_vbo = 0;
    m_vao = 0;
    m_initialized = false;
}

// --- projection / transform ---------------------------------------------------

void Renderer2D::setOrtho(float l, float r, float b, float t)
{
    m_projection = Mat4::ortho(l, r, b, t);
}

void Renderer2D::setProjection(const Mat4 &projection)
{
    m_projection = projection;
}

void Renderer2D::loadIdentity()
{
    m_transformStack.back().identity();
}

void Renderer2D::pushTransform()
{
    m_transformStack.push_back(m_transformStack.back());
}

void Renderer2D::popTransform()
{
    if (m_transformStack.size() > 1)
        m_transformStack.pop_back();
}

void Renderer2D::translate(float x, float y)
{
    m_transformStack.back() = m_transformStack.back() * Mat4::translation(x, y);
}

void Renderer2D::rotateZ(float degrees)
{
    m_transformStack.back() = m_transformStack.back() * Mat4::rotationZ(degrees);
}

void Renderer2D::scale(float sx, float sy)
{
    m_transformStack.back() = m_transformStack.back() * Mat4::scaling(sx, sy);
}

void Renderer2D::resetTransform()
{
    m_transformStack.assign(1, Mat4());
}

// --- GL state helpers ---------------------------------------------------------

void Renderer2D::setViewport(int x, int y, int w, int h)
{
    m_viewportW = w;
    m_viewportH = h;
    QOpenGLExtraFunctions *f = gl();
    if (f)
        f->glViewport(x, y, w, h);
}

void Renderer2D::setScissor(int x, int y, int w, int h)
{
    QOpenGLExtraFunctions *f = gl();
    if (f)
        f->glScissor(x, y, w, h);
}

void Renderer2D::setScissorEnabled(bool enable)
{
    QOpenGLExtraFunctions *f = gl();
    if (!f)
        return;
    if (enable)
        f->glEnable(GL_SCISSOR_TEST);
    else
        f->glDisable(GL_SCISSOR_TEST);
}

void Renderer2D::clear(float r, float g, float b, float a)
{
    QOpenGLExtraFunctions *f = gl();
    if (!f)
        return;
    f->glClearColor(r, g, b, a);
    f->glClear(GL_COLOR_BUFFER_BIT);
}

// --- batching -----------------------------------------------------------------

void Renderer2D::begin(Primitive primitive)
{
    m_primitive = primitive;
    m_inBatch = true;
    m_lineWidth = 1.0f;
    m_dashed = false;
    m_vertices.clear();
}

void Renderer2D::beginLines(float width)
{
    begin(Lines);
    m_lineWidth = width;
}

void Renderer2D::beginDashedLines(float width, float periodPixels)
{
    begin(Lines);
    m_lineWidth = width;
    m_dashed = true;
    m_dashPeriod = periodPixels > 0.0f ? periodPixels : 16.0f;
}

void Renderer2D::beginLineStrip(float width)
{
    begin(LineStrip);
    m_lineWidth = width;
}

void Renderer2D::beginLineLoop(float width)
{
    begin(LineLoop);
    m_lineWidth = width;
}

void Renderer2D::color(float r, float g, float b, float a)
{
    m_color[0] = r;
    m_color[1] = g;
    m_color[2] = b;
    m_color[3] = a;
}

void Renderer2D::texCoord(float u, float v)
{
    m_tex[0] = u;
    m_tex[1] = v;
}

void Renderer2D::vertex(float x, float y)
{
    m_vertices.push_back(x);
    m_vertices.push_back(y);
    m_vertices.push_back(m_color[0]);
    m_vertices.push_back(m_color[1]);
    m_vertices.push_back(m_color[2]);
    m_vertices.push_back(m_color[3]);
    m_vertices.push_back(m_tex[0]);
    m_vertices.push_back(m_tex[1]);
}

void Renderer2D::end()
{
    m_inBatch = false;

    if (m_vertices.empty() || !ensureInit())
    {
        m_vertices.clear();
        return;
    }

    QOpenGLExtraFunctions *f = gl();
    if (f == nullptr)
    {
        m_vertices.clear();
        return;
    }

    // Expand emulated quads (v0,v1,v2,v3) into two triangles (0,1,2, 0,2,3).
    std::vector<float> expanded;
    const std::vector<float> *toDraw = &m_vertices;
    unsigned int drawMode = GL_TRIANGLES;
    int vertexCount = (int)(m_vertices.size() / kFloatsPerVertex);

    switch (m_primitive)
    {
    case Quads:
    {
        int quads = vertexCount / 4;
        expanded.reserve(quads * 6 * kFloatsPerVertex);
        auto copyVertex = [&](int index) {
            const float *src = &m_vertices[index * kFloatsPerVertex];
            expanded.insert(expanded.end(), src, src + kFloatsPerVertex);
        };
        for (int q = 0; q < quads; ++q)
        {
            int base = q * 4;
            copyVertex(base + 0);
            copyVertex(base + 1);
            copyVertex(base + 2);
            copyVertex(base + 0);
            copyVertex(base + 2);
            copyVertex(base + 3);
        }
        toDraw = &expanded;
        vertexCount = quads * 6;
        drawMode = GL_TRIANGLES;
        break;
    }
    case Triangles:      drawMode = GL_TRIANGLES;      break;
    case TriangleStrip:  drawMode = GL_TRIANGLE_STRIP; break;
    case TriangleFan:    drawMode = GL_TRIANGLE_FAN;   break;
    case Lines:          drawMode = GL_LINES;          break;
    case LineStrip:      drawMode = GL_LINE_STRIP;     break;
    case LineLoop:       drawMode = GL_LINE_LOOP;      break;
    case Points:         drawMode = GL_POINTS;         break;
    }

    // Dashed lines: store each segment's along-length (pixels) in tex.x so the
    // fragment shader can discard the "off" portion. Segments are vertex pairs.
    if (m_dashed && drawMode == GL_LINES)
    {
        for (int v = 0; v + 1 < vertexCount; v += 2)
        {
            float *a = &m_vertices[v * kFloatsPerVertex];
            float *b = &m_vertices[(v + 1) * kFloatsPerVertex];
            const float dx = b[0] - a[0];
            const float dy = b[1] - a[1];
            const float len = std::sqrt(dx * dx + dy * dy);
            a[6] = 0.0f;
            b[6] = len;
        }
    }

    // Thick lines: glLineWidth > 1 is clamped to 1 in a core profile / WebGL, so
    // expand line segments into quads (two triangles each). Handles GL_LINES,
    // GL_LINE_STRIP and GL_LINE_LOOP (so ring/outline overlays thicken too). The
    // requested width is in device pixels; convert to the local (logical/model)
    // unit via the projection + viewport scale so thickness is DPR- and
    // zoom-correct. Strips/loops get square caps so segments bridge at joints.
    if ((drawMode == GL_LINES || drawMode == GL_LINE_STRIP || drawMode == GL_LINE_LOOP)
        && m_lineWidth > 1.0f)
    {
        float unitsPerPixel = 1.0f;
        if (m_viewportW > 0 && m_projection.m[0] != 0.0f)
            unitsPerPixel = 2.0f / (m_projection.m[0] * (float)m_viewportW);
        const float halfW = m_lineWidth * unitsPerPixel * 0.5f;
        const bool cap = (drawMode != GL_LINES);

        expanded.clear();
        expanded.reserve((size_t)vertexCount * 6 * kFloatsPerVertex);
        auto emitCorner = [&](const float *src, float px, float py) {
            expanded.push_back(px);
            expanded.push_back(py);
            expanded.push_back(src[2]);
            expanded.push_back(src[3]);
            expanded.push_back(src[4]);
            expanded.push_back(src[5]);
            expanded.push_back(src[6]);
            expanded.push_back(src[7]);
        };
        auto expandSeg = [&](int ia, int ib) {
            const float *a = &m_vertices[ia * kFloatsPerVertex];
            const float *b = &m_vertices[ib * kFloatsPerVertex];
            const float dx = b[0] - a[0];
            const float dy = b[1] - a[1];
            const float len = std::sqrt(dx * dx + dy * dy);
            if (len < 1e-6f)
                return;
            const float ux = dx / len, uy = dy / len; // unit direction
            const float nx = -uy * halfW, ny = ux * halfW; // perpendicular
            const float ex = cap ? ux * halfW : 0.0f;      // square-cap extension
            const float ey = cap ? uy * halfW : 0.0f;
            const float ax = a[0] - ex, ay = a[1] - ey;
            const float bx = b[0] + ex, by = b[1] + ey;
            emitCorner(a, ax + nx, ay + ny);
            emitCorner(a, ax - nx, ay - ny);
            emitCorner(b, bx - nx, by - ny);
            emitCorner(a, ax + nx, ay + ny);
            emitCorner(b, bx - nx, by - ny);
            emitCorner(b, bx + nx, by + ny);
        };

        if (drawMode == GL_LINES)
        {
            for (int v = 0; v + 1 < vertexCount; v += 2)
                expandSeg(v, v + 1);
        }
        else // GL_LINE_STRIP / GL_LINE_LOOP
        {
            for (int v = 0; v + 1 < vertexCount; ++v)
                expandSeg(v, v + 1);
            if (drawMode == GL_LINE_LOOP && vertexCount > 2)
                expandSeg(vertexCount - 1, 0);
        }

        toDraw = &expanded;
        vertexCount = (int)(expanded.size() / kFloatsPerVertex);
        drawMode = GL_TRIANGLES;
    }

    m_program->use();
    m_program->setMat4("uMVP", m_projection * currentTransform());
    m_program->setInt("uUseTexture", 0);
    m_program->setInt("uForceOpaque", m_forceOpaque ? 1 : 0);
    m_program->setInt("uDashed", m_dashed ? 1 : 0);
    m_program->setFloat("uDashPeriod", m_dashPeriod);

    if (drawMode == GL_LINES || drawMode == GL_LINE_STRIP || drawMode == GL_LINE_LOOP)
        f->glLineWidth(m_lineWidth); // clamped to 1.0 on WebGL / some core drivers

    f->glBindVertexArray(m_vao);
    f->glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    f->glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(toDraw->size() * sizeof(float)),
                    toDraw->data(), GL_DYNAMIC_DRAW);
    f->glDrawArrays(drawMode, 0, vertexCount);
    f->glBindVertexArray(0);
    f->glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Restore the fixed-function pipeline. During the migration, immediate-mode
    // code (FemGrid2, glDrawPixels in PaintView) still runs between Renderer2D
    // draws; in a compatibility context a left-bound program would also process
    // those fragments and render them black.
    f->glUseProgram(0);

    m_vertices.clear();
}

// --- textured quad ------------------------------------------------------------

void Renderer2D::drawTexturedQuad(unsigned int texture,
                                  float x, float y, float w, float h,
                                  float u0, float v0, float u1, float v1,
                                  float tintR, float tintG, float tintB, float tintA)
{
    if (!ensureInit())
        return;

    QOpenGLExtraFunctions *f = gl();
    if (f == nullptr)
        return;

    const float verts[] = {
        // x,      y,       r,     g,     b,     a,     u,   v
        x,     y,     tintR, tintG, tintB, tintA, u0, v0,
        x + w, y,     tintR, tintG, tintB, tintA, u1, v0,
        x + w, y + h, tintR, tintG, tintB, tintA, u1, v1,
        x,     y,     tintR, tintG, tintB, tintA, u0, v0,
        x + w, y + h, tintR, tintG, tintB, tintA, u1, v1,
        x,     y + h, tintR, tintG, tintB, tintA, u0, v1,
    };

    m_program->use();
    m_program->setMat4("uMVP", m_projection * currentTransform());
    m_program->setInt("uUseTexture", 1);
    m_program->setInt("uForceOpaque", m_forceOpaque ? 1 : 0);
    m_program->setInt("uDashed", 0);
    m_program->setFloat("uDashPeriod", m_dashPeriod);
    m_program->setInt("uTex", 0);

    f->glActiveTexture(GL_TEXTURE0);
    f->glBindTexture(GL_TEXTURE_2D, texture);

    f->glBindVertexArray(m_vao);
    f->glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    f->glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
    f->glDrawArrays(GL_TRIANGLES, 0, 6);
    f->glBindVertexArray(0);
    f->glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Restore fixed-function state for interleaved immediate-mode rendering
    // (see note in end()).
    f->glBindTexture(GL_TEXTURE_2D, 0);
    f->glUseProgram(0);
}

void Renderer2D::setForceOpaque(bool enable)
{
    m_forceOpaque = enable;
}

void Renderer2D::setBlend(bool enable)
{
    QOpenGLExtraFunctions *f = gl();
    if (f == nullptr)
        return;
    if (enable)
    {
        f->glEnable(GL_BLEND);
        f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else
    {
        f->glDisable(GL_BLEND);
    }
}

} // namespace ivf2d
