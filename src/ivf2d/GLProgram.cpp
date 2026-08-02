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

#include "GLProgram.h"

#include <QOpenGLContext>

#include <vector>

#include "FPLog.h"
#include <string>

namespace ivf2d {

QOpenGLExtraFunctions *GLProgram::gl()
{
    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    return ctx ? ctx->extraFunctions() : nullptr;
}

std::string GLProgram::glslHeader()
{
    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    if (ctx && ctx->isOpenGLES())
        return "#version 300 es\nprecision highp float;\n";
    return "#version 330 core\n";
}

GLProgram::GLProgram() = default;

GLProgram::~GLProgram()
{
    destroy();
}

unsigned int GLProgram::compileStage(unsigned int type, const std::string &source)
{
    QOpenGLExtraFunctions *f = gl();
    if (f == nullptr)
        return 0;

    const std::string full = glslHeader() + source;
    const char *src = full.c_str();

    unsigned int shader = f->glCreateShader(type);
    f->glShaderSource(shader, 1, &src, nullptr);
    f->glCompileShader(shader);

    int compiled = 0;
    f->glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        int logLength = 0;
        f->glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> log(logLength > 1 ? logLength : 1);
        f->glGetShaderInfoLog(shader, (int)log.size(), nullptr, log.data());
        fp_error("GLProgram", "Shader compile failed: {}", log.data());
        f->glDeleteShader(shader);
        return 0;
    }
    return shader;
}

bool GLProgram::compile(const std::string &vertexSource, const std::string &fragmentSource)
{
    QOpenGLExtraFunctions *f = gl();
    if (f == nullptr)
    {
        fp_error("GLProgram", "compile() called with no current GL context");
        return false;
    }

    destroy();

    unsigned int vs = compileStage(GL_VERTEX_SHADER, vertexSource);
    if (vs == 0)
        return false;

    unsigned int fs = compileStage(GL_FRAGMENT_SHADER, fragmentSource);
    if (fs == 0)
    {
        f->glDeleteShader(vs);
        return false;
    }

    unsigned int program = f->glCreateProgram();
    f->glAttachShader(program, vs);
    f->glAttachShader(program, fs);
    f->glLinkProgram(program);

    // Shaders are no longer needed once linked.
    f->glDeleteShader(vs);
    f->glDeleteShader(fs);

    int linked = 0;
    f->glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        int logLength = 0;
        f->glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> log(logLength > 1 ? logLength : 1);
        f->glGetProgramInfoLog(program, (int)log.size(), nullptr, log.data());
        fp_error("GLProgram", "Program link failed: {}", log.data());
        f->glDeleteProgram(program);
        return false;
    }

    m_program = program;
    return true;
}

void GLProgram::use()
{
    QOpenGLExtraFunctions *f = gl();
    if (f && m_program)
        f->glUseProgram(m_program);
}

void GLProgram::destroy()
{
    QOpenGLExtraFunctions *f = gl();
    if (f && m_program)
        f->glDeleteProgram(m_program);
    m_program = 0;
}

int GLProgram::uniformLocation(const char *name)
{
    QOpenGLExtraFunctions *f = gl();
    return (f && m_program) ? f->glGetUniformLocation(m_program, name) : -1;
}

int GLProgram::attribLocation(const char *name)
{
    QOpenGLExtraFunctions *f = gl();
    return (f && m_program) ? f->glGetAttribLocation(m_program, name) : -1;
}

void GLProgram::setMat4(const char *name, const Mat4 &matrix)
{
    QOpenGLExtraFunctions *f = gl();
    if (!f || !m_program)
        return;
    int loc = f->glGetUniformLocation(m_program, name);
    if (loc >= 0)
        f->glUniformMatrix4fv(loc, 1, GL_FALSE, matrix.data());
}

void GLProgram::setInt(const char *name, int value)
{
    QOpenGLExtraFunctions *f = gl();
    if (!f || !m_program)
        return;
    int loc = f->glGetUniformLocation(m_program, name);
    if (loc >= 0)
        f->glUniform1i(loc, value);
}

void GLProgram::setFloat(const char *name, float value)
{
    QOpenGLExtraFunctions *f = gl();
    if (!f || !m_program)
        return;
    int loc = f->glGetUniformLocation(m_program, name);
    if (loc >= 0)
        f->glUniform1f(loc, value);
}

void GLProgram::setVec4(const char *name, float r, float g, float b, float a)
{
    QOpenGLExtraFunctions *f = gl();
    if (!f || !m_program)
        return;
    int loc = f->glGetUniformLocation(m_program, name);
    if (loc >= 0)
        f->glUniform4f(loc, r, g, b, a);
}

} // namespace ivf2d
