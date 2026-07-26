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

#include "StreamTexture.h"

#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>

#include "Renderer2D.h"

namespace ivf2d {

QOpenGLExtraFunctions *StreamTexture::gl()
{
    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    return ctx ? ctx->extraFunctions() : nullptr;
}

StreamTexture::~StreamTexture()
{
    destroy();
}

static unsigned int sizedInternalFormat(unsigned int format)
{
    // Sized internal formats are required for texture completeness in
    // WebGL 2 / OpenGL ES 3.0 and are valid in desktop core profiles too.
    return (format == GL_RGB) ? GL_RGB8 : GL_RGBA8;
}

void StreamTexture::update(const unsigned char *pixels, int width, int height,
                           unsigned int format)
{
    QOpenGLExtraFunctions *f = gl();
    if (f == nullptr || pixels == nullptr || width <= 0 || height <= 0)
        return;

    const bool needAlloc =
        (m_texture == 0) || (width != m_width) || (height != m_height) || (format != m_format);

    if (m_texture == 0)
        f->glGenTextures(1, &m_texture);

    f->glActiveTexture(GL_TEXTURE0);
    f->glBindTexture(GL_TEXTURE_2D, m_texture);
    f->glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if (needAlloc)
    {
        f->glTexImage2D(GL_TEXTURE_2D, 0, sizedInternalFormat(format),
                        width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
        const int filter = m_linear ? GL_LINEAR : GL_NEAREST;
        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        m_width = width;
        m_height = height;
        m_format = format;
    }
    else
    {
        f->glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
                           format, GL_UNSIGNED_BYTE, pixels);
    }

    f->glBindTexture(GL_TEXTURE_2D, 0);
}

void StreamTexture::draw(float x, float y, float width, float height,
                         float tintR, float tintG, float tintB, float tintA)
{
    if (m_texture == 0)
        return;
    Renderer2D::instance().drawTexturedQuad(m_texture, x, y, width, height,
                                            0.0f, 0.0f, 1.0f, 1.0f,
                                            tintR, tintG, tintB, tintA);
}

void StreamTexture::drawZoomed(float x, float y, float zoom)
{
    draw(x, y, m_width * zoom, m_height * zoom);
}

void StreamTexture::setLinearFilter(bool linear)
{
    if (m_linear == linear)
        return;
    m_linear = linear;

    QOpenGLExtraFunctions *f = gl();
    if (f == nullptr || m_texture == 0)
        return;
    const int filter = linear ? GL_LINEAR : GL_NEAREST;
    f->glBindTexture(GL_TEXTURE_2D, m_texture);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    f->glBindTexture(GL_TEXTURE_2D, 0);
}

void StreamTexture::destroy()
{
    QOpenGLExtraFunctions *f = gl();
    if (f && m_texture)
        f->glDeleteTextures(1, &m_texture);
    m_texture = 0;
    m_width = 0;
    m_height = 0;
    m_format = 0;
}

} // namespace ivf2d
