//
// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2003 Division of Structural Mecahnics, Lund University
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

#ifndef _Arrow_h_
#define _Arrow_h_

#include "Shape.h"
#include "Line.h"
#include "Vec3d.h"

namespace ivf2d {

IvfSmartPointer(Arrow);

class Arrow : public Shape {
private:
	Line* m_line;
	Line* m_head1;
	Line* m_head2;
public:
	Arrow();
	virtual ~Arrow();

	static ArrowPtr create() { return std::make_shared<Arrow>(); }

	IvfClassInfo("Arrow",Shape);
};


} // namespace ivf2d

#endif 
