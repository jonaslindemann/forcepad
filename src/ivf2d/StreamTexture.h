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

// Forward-declared so this header carries no Qt dependency and can be included
// by framework-agnostic consumers. The Qt-backed implementation is in the .cpp.
class QOpenGLExtraFunctions;

namespace ivf2d {

// Source pixel-format constants (values match GL_RGBA / GL_RGB) so this header
// needs no GL include.
enum : unsigned int {
    ST_RGBA = 0x1908,
    ST_RGB  = 0x1907
};

/**
 * A GL texture streamed from a CPU pixel buffer, drawn as a textured quad.
 *
 * This is the replacement for glDrawPixels / glRasterPos / glPixelZoom, none of
 * which exist in an OpenGL core profile or in WebGL / OpenGL ES. ForcePAD's
 * paint engine is CPU-side raster (ImageGrid / ScreenImage), so every on-screen
 * bitmap - the drawing canvas, the clipboard preview, the FEM stress-field image
 * - is uploaded here and blitted with Renderer2D.
 *
 * update() (re)allocates the texture with glTexImage2D when the size or format
 * changes and uses the cheaper glTexSubImage2D otherwise, so streaming a
 * same-size buffer each frame stays fast. Filtering defaults to GL_NEAREST for
 * pixel-accurate blits.
 */
class StreamTexture {
public:
    StreamTexture() = default;
    ~StreamTexture();

    /**
     * Uploads pixels of the given size and format.
     * @param format one of GL_RGBA or GL_RGB (source layout).
     */
    void update(const unsigned char *pixels, int width, int height,
                unsigned int format = ST_RGBA);

    /** Draws the texture at (x, y) with the given on-screen size via Renderer2D. */
    void draw(float x, float y, float width, float height,
              float tintR = 1.0f, float tintG = 1.0f,
              float tintB = 1.0f, float tintA = 1.0f);

    /** Convenience: draw at native pixel size scaled by zoom. */
    void drawZoomed(float x, float y, float zoom = 1.0f);

    void setLinearFilter(bool linear);

    unsigned int id() const { return m_texture; }
    int width() const { return m_width; }
    int height() const { return m_height; }

    /** Releases GL resources. Call with the owning context current. */
    void destroy();

private:
    static QOpenGLExtraFunctions *gl();

    unsigned int m_texture{0};
    int m_width{0};
    int m_height{0};
    unsigned int m_format{0};
    bool m_linear{false};
};

} // namespace ivf2d
