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

#ifdef __APPLE__
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

#include "Vec3d.h"

CArch::CArch()
{
    m_size[0] = 1.0;
    m_size[1] = 1.0;

    m_lineWidth = 1.0;
    m_lineColor = new CColor();
    m_lineColor->setColor(0.0f, 0.0f, 0.0f, 1.0f);
    m_ellipse = new CEllipse();
}

CArch::~CArch()
{

}

void CArch::setSize(double width, double height)
{
    m_size[0] = width;
    m_size[1] = height;
}

void CArch::getSize(double &width, double &height)
{
    width = m_size[0];
    height = m_size[1];
}

double CArch::f1(double x)
{
    return m_size[1]-(4*m_size[1])*pow(x,2.0)/pow(m_size[0],2.0);
}

void CArch::doGeometry()
{
    double x, y;
    double dX = m_size[0]/20.0;

    CVec3d forward;
    CVec3d right;
    CVec3d p1;
    CVec3d p2;
    CVec3d qp1, qp2, qp3, qp4;

    double dx, dy, dz;

    x = -m_size[0]/2.0;
    y = f1(x);

    while (x<m_size[0]/2.0-dX*0.5)
    {
        y = f1(x);
        p1.setComponents(m_size[0]/2.0 + x, y);

        y = f1(x + dX);
        p2.setComponents(m_size[0]/2.0 + x + dX, y);

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

        qp1 = p1 + (double)m_lineWidth*0.5*right;
        qp2 = p1 - (double)m_lineWidth*0.5*right;
        qp3 = p2 - (double)m_lineWidth*0.5*right;
        qp4 = p2 + (double)m_lineWidth*0.5*right;

        glBegin(GL_QUADS);
        glVertex2d(qp1.getX(), qp1.getY());
        glVertex2d(qp2.getX(), qp2.getY());
        glVertex2d(qp3.getX(), qp3.getY());
        glVertex2d(qp4.getX(), qp4.getY());
        glEnd();


        x += dX;

    }

    /*
    glBegin(GL_LINE_STRIP);
    for (x=-m_size[0]/2.0; x<m_size[0]/2.0; x+=1.0)
    {
        y = m_size[1]-(4*m_size[1])*pow(x,2.0)/pow(m_size[0],2.0);
        glVertex2d(x+m_size[0]/2.0, y);
    }
    glEnd();
    */
}

void CArch::setLineWidth(double width)
{
    m_lineWidth = width;
}

double CArch::getLineWidth()
{
    return m_lineWidth;
}


void CArch::setLineColor(CColor *color)
{
    m_lineColor = color;
}
