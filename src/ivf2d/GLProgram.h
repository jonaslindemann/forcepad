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

#include <QOpenGLExtraFunctions>

#include <string>

#include "Mat4.h"

namespace ivf2d {

/**
 * Modern shader-program wrapper (VBO + shader path).
 *
 * Part of the rendering migration away from fixed-function OpenGL. Compiles a
 * vertex + fragment shader pair and links them into a program that is portable
 * across desktop OpenGL 3.3 Core and WebGL 2 / OpenGL ES 3.0.
 *
 * The GLSL sources passed to compile() must NOT contain a "#version" line; the
 * correct version + precision header for the current context (desktop core vs.
 * GLES) is prepended automatically by glslHeader(). This lets one set of shader
 * sources target both the desktop build and the future WebAssembly build.
 *
 * GL entry points are resolved through Qt's QOpenGLExtraFunctions, which works
 * identically on desktop and under Qt for WebAssembly, so no separate GL loader
 * (GLAD/GLEW) is required.
 */
class GLProgram {
public:
    GLProgram();
    ~GLProgram();

    /** Compiles + links the given sources (no "#version" line). Returns success. */
    bool compile(const std::string &vertexSource, const std::string &fragmentSource);

    /** Makes this program current. */
    void use();

    /** Deletes GL resources. Safe to call with no live GL context (no-op). */
    void destroy();

    bool isValid() const { return m_program != 0; }
    unsigned int id() const { return m_program; }

    int uniformLocation(const char *name);
    int attribLocation(const char *name);

    void setMat4(const char *name, const Mat4 &matrix);
    void setInt(const char *name, int value);
    void setFloat(const char *name, float value);
    void setVec4(const char *name, float r, float g, float b, float a);

    /**
     * Returns the version + precision header for the current GL context:
     *   desktop -> "#version 330 core\n"
     *   GLES / WebGL 2 -> "#version 300 es\nprecision highp float;\n"
     */
    static std::string glslHeader();

private:
    static QOpenGLExtraFunctions *gl();
    unsigned int compileStage(unsigned int type, const std::string &source);

    unsigned int m_program{0};
};

} // namespace ivf2d
