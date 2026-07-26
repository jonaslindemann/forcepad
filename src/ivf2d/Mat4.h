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

#include <cmath>

namespace ivf2d {

/**
 * Minimal column-major 4x4 float matrix for 2D rendering.
 *
 * Replaces the fixed-function matrix stack (glMatrixMode / gluOrtho2D /
 * glPushMatrix / glTranslated / glRotatef) that is unavailable in an OpenGL
 * core profile and in WebGL / OpenGL ES. Column-major storage matches the
 * layout GLSL expects for a mat4 uniform, so the raw data() pointer can be
 * handed straight to glUniformMatrix4fv with transpose = GL_FALSE.
 *
 * Header-only and free of any GL / Qt dependency so it is usable from every
 * ForcePAD target.
 */
class Mat4 {
public:
    // Column-major: m[col*4 + row]
    float m[16];

    Mat4() { identity(); }

    void identity()
    {
        for (int i = 0; i < 16; ++i)
            m[i] = 0.0f;
        m[0] = m[5] = m[10] = m[15] = 1.0f;
    }

    const float *data() const { return m; }

    /** 2D orthographic projection matching gluOrtho2D(l, r, b, t). */
    static Mat4 ortho(float l, float r, float b, float t,
                      float nearZ = -1.0f, float farZ = 1.0f)
    {
        Mat4 result;
        result.identity();
        result.m[0]  = 2.0f / (r - l);
        result.m[5]  = 2.0f / (t - b);
        result.m[10] = -2.0f / (farZ - nearZ);
        result.m[12] = -(r + l) / (r - l);
        result.m[13] = -(t + b) / (t - b);
        result.m[14] = -(farZ + nearZ) / (farZ - nearZ);
        return result;
    }

    static Mat4 translation(float x, float y, float z = 0.0f)
    {
        Mat4 result;
        result.m[12] = x;
        result.m[13] = y;
        result.m[14] = z;
        return result;
    }

    static Mat4 scaling(float sx, float sy, float sz = 1.0f)
    {
        Mat4 result;
        result.m[0]  = sx;
        result.m[5]  = sy;
        result.m[10] = sz;
        return result;
    }

    /** Rotation about the Z axis (degrees), matching glRotatef(a, 0, 0, 1). */
    static Mat4 rotationZ(float degrees)
    {
        const float rad = degrees * 3.14159265358979323846f / 180.0f;
        const float c = std::cos(rad);
        const float s = std::sin(rad);
        Mat4 result;
        result.m[0] = c;
        result.m[1] = s;
        result.m[4] = -s;
        result.m[5] = c;
        return result;
    }

    /** Matrix product (this * rhs), column-major. */
    Mat4 operator*(const Mat4 &rhs) const
    {
        Mat4 result;
        for (int col = 0; col < 4; ++col)
        {
            for (int row = 0; row < 4; ++row)
            {
                float sum = 0.0f;
                for (int k = 0; k < 4; ++k)
                    sum += m[k * 4 + row] * rhs.m[col * 4 + k];
                result.m[col * 4 + row] = sum;
            }
        }
        return result;
    }
};

} // namespace ivf2d
