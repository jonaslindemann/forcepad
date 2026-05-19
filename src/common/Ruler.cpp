#include "Ruler.h"
#include "UiSettings.h"

#ifdef __APPLE__
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

#include "Vec3d.h"

namespace fp {

Ruler::Ruler ()
		:ivf2d::Shape()
{
	m_startPos[0] = 0;
	m_startPos[1] = 0;
	m_endPos[0] = 1;
	m_endPos[1] = 0;
	m_actualLength = 1.0;
}

Ruler::~Ruler ()
{

}

void Ruler::initRuler()
{
}

void Ruler::doGeometry()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glLineWidth(2.0f * (float)UiSettings::getInstance()->getDevicePixelRatio());

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

void Ruler::setEndPos(int x, int y)
{
	m_endPos[0] = x;
	m_endPos[1] = y;
}

void Ruler::setStartPos(int x, int y)
{
	m_startPos[0] = x;
	m_startPos[1] = y;
}

void Ruler::setActualLength(double length)
{
	m_actualLength = length;
}

double Ruler::getActualLength()
{
	return m_actualLength;
}

double Ruler::getPixelLength()
{
	ivf2d::Vec3d p1;
	ivf2d::Vec3d p2;
	ivf2d::Vec3d v;

	p1.setComponents(m_startPos);
	p2.setComponents(m_endPos);

	v = p2-p1;
	return v.length();
}

} // namespace fp
