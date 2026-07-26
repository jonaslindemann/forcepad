#include "Ruler.h"
#include "UiSettings.h"

#include "Renderer2D.h"
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
	const float lineWidth = 2.0f * (float)UiSettings::getInstance()->getDevicePixelRatio();
	ivf2d::Renderer2D &r = ivf2d::Renderer2D::instance();

	// End cross-hairs (solid).
	r.beginLines(lineWidth);
	r.vertex((float)m_startPos[0], (float)(m_startPos[1]-10));
	r.vertex((float)m_startPos[0], (float)(m_startPos[1]+10));
	r.vertex((float)(m_startPos[0]-10), (float)m_startPos[1]);
	r.vertex((float)(m_startPos[0]+10), (float)m_startPos[1]);
	r.vertex((float)m_endPos[0], (float)(m_endPos[1]-10));
	r.vertex((float)m_endPos[0], (float)(m_endPos[1]+10));
	r.vertex((float)(m_endPos[0]-10), (float)m_endPos[1]);
	r.vertex((float)(m_endPos[0]+10), (float)m_endPos[1]);
	r.end();

	// Measuring line (was glLineStipple(1, 0x3333) -> 2 px on / 2 px off).
	r.beginDashedLines(lineWidth, 4.0f);
	r.vertex((float)m_startPos[0], (float)m_startPos[1]);
	r.vertex((float)m_endPos[0], (float)m_endPos[1]);
	r.end();
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
