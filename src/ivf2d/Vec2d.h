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
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//
// Comments and suggestions to jonas.lindemann@byggmek.lth.se
//

#pragma once

namespace ivf2d {

/**
 * Plain 2D value pair.
 *
 * Return type for the accessors that used to hand back a pair of values through
 * reference out-parameters. A trivial aggregate on purpose -- no constructors,
 * no virtuals -- so it costs nothing to return by value. (Vec3d is the 3D
 * vector-maths type; this is deliberately just a pair of coordinates.)
 */
struct Vec2d {
    double x{0.0};
    double y{0.0};
};

/** Integer rectangle, for the clipboard selection. */
struct Rect2i {
    int x1{0};
    int y1{0};
    int x2{0};
    int y2{0};
};

} // namespace ivf2d
