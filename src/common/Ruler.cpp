#include "Ruler.h"

#ifdef __APPLE__
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

#include "Vec3d.h"

CRuler::CRuler ()
		:CShape()
{
	m_startPos[0] = 0;
	m_startPos[1] = 0;
	m_endPos[0] = 1;
	m_endPos[1] = 0;
	m_actualLength = 1.0;
}

CRuler::~CRuler ()
{

}

void CRuler::initRuler()
{
}

void CRuler::doGeometry()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glLineWidth(2);

	glBegin(GL_LINES);
	glVertex2i(m_startPos[0], m_startPos[1]-10);
	glVertex2i(m_startPos[0], m_startPos[1]+10);
	glVertex2i(m_startPos[0]-10, m_startPos[1]);
	glVertex2i(m_startPos[0]+10, m_startPos[1]);
	glVertex2i(m_endPos[0], m_endPos[1]-10);
	glVertex2i(m_endPos[0], m_endPos[1]+10);
	glVertex2i(m_endPos[0]-10, m_endPos[1]);
	glVertex2i(m_endPos[0]+10, m_endPos[1]);
	glEnd();

	glLineStipple(1, 0x3333);
	glEnable(GL_LINE_STIPPLE);
	glBegin(GL_LINES);
		glVertex2iv((GLint*)m_startPos);
		glVertex2iv((GLint*)m_endPos);
	glEnd();
	glPopAttrib();
}

void CRuler::setEndPos(int x, int y)
{
	m_endPos[0] = x;
	m_endPos[1] = y;
}

void CRuler::setStartPos(int x, int y)
{
	m_startPos[0] = x;
	m_startPos[1] = y;
}

void CRuler::setActualLength(double length)
{
	m_actualLength = length;
}

double CRuler::getActualLength()
{
	return m_actualLength;
}

double CRuler::getPixelLength()
{
	CVec3d p1;
	CVec3d p2;
	CVec3d v;

	p1.setComponents(m_startPos);
	p2.setComponents(m_endPos);

	v = p2-p1;
	return v.length();
}

