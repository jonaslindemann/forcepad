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

#include "Arch.h"

#include "Renderer2D.h"
#include "Vec3d.h"

namespace ivf2d {

Arch::Arch()
{
    m_size[0] = 1.0;
    m_size[1] = 1.0;

    m_lineWidth = 1.0;
    m_lineColor = Color::create();
    m_lineColor->setColor(0.0f, 0.0f, 0.0f, 1.0f);
    m_ellipse = Ellipse::create();
}

Arch::~Arch()
{

}

void Arch::setSize(double width, double height)
{
    m_size[0] = width;
    m_size[1] = height;
}

Vec2d Arch::getSize() const
{
    return {m_size[0], m_size[1]};
}

double Arch::f1(double x)
{
    return m_size[1]-(4*m_size[1])*x*x/(m_size[0]*m_size[0]);
}

void Arch::doGeometry()
{
    double x, y;
    double dX = m_size[0]/20.0;

    Vec3d forward;
    Vec3d right;
    Vec3d p1;
    Vec3d p2;
    Vec3d qp1, qp2, qp3, qp4;

    double dx, dy, dz;

    x = -m_size[0]/2.0;
    y = f1(x);

    while (x<m_size[0]/2.0-dX*0.5)
    {
        // The three-argument (double) overload: the two-argument one takes
        // ints, so these curve coordinates were silently truncated to whole
        // pixels before -- which /W4 surfaced as C4244.
        y = f1(x);
        p1.setComponents(m_size[0]/2.0 + x, y, 0.0);

        y = f1(x + dX);
        p2.setComponents(m_size[0]/2.0 + x + dX, y, 0.0);

        if (x<m_size[0]/2.0-dX*1.5)
        {
            m_ellipse->setSizeDouble(m_lineWidth/2.0, m_lineWidth/2.0);
            m_ellipse->setPosition(m_size[0]/2.0+x + dX , y);
            m_ellipse->render();
        }

        forward.setFromPoints(p1,p2);
        forward.normalize();

        forward.getComponents(dx, dy, dz);
        right.setComponents(-dy, dx, 0.0);

        qp1 = p1 + static_cast<double>(m_lineWidth)*0.5*right;
        qp2 = p1 - static_cast<double>(m_lineWidth)*0.5*right;
        qp3 = p2 - static_cast<double>(m_lineWidth)*0.5*right;
        qp4 = p2 + static_cast<double>(m_lineWidth)*0.5*right;

        Renderer2D &r = Renderer2D::instance();
        r.beginQuads();
        r.vertex((float)qp1.getX(), (float)qp1.getY());
        r.vertex((float)qp2.getX(), (float)qp2.getY());
        r.vertex((float)qp3.getX(), (float)qp3.getY());
        r.vertex((float)qp4.getX(), (float)qp4.getY());
        r.end();


        x += dX;

    }
}

void Arch::setLineWidth(double width)
{
    m_lineWidth = width;
}

double Arch::getLineWidth() const
{
    return m_lineWidth;
}


void Arch::setLineColor(ColorPtr color)
{
    m_lineColor = color;
}

} // namespace ivf2d
