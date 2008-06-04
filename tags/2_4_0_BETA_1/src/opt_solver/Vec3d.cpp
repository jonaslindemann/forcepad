#include "Vec3d.h"

//
// From Stroustrup 11.6
//

const int rfemMaxTempVec3d = 32;

CVec3d::CVec3d()
{
	setComponents(0.0, 0.0, 0.0);
}

CVec3d& rfemGetTempVec3d()
{
	static int nBuf = 0;
	static CVec3d buf[rfemMaxTempVec3d];
	
	if (nBuf==rfemMaxTempVec3d) nBuf = 0;

	buf[nBuf].setComponents(0.0, 0.0, 0.0);

	return buf[nBuf++];
}

CVec3d::CVec3d(double vx, double vy, double vz)
{
	m_vector[0] = vx;
	m_vector[1] = vy;
	m_vector[2] = vz;
}

CVec3d::CVec3d(CVec3d& v)
{
	v.getComponents(m_vector);
}

CVec3d::~CVec3d()
{

}

void CVec3d::setComponents(double vx, double vy, double vz)
{
	m_vector[0] = vx;
	m_vector[1] = vy;
	m_vector[2] = vz;
}

void CVec3d::setComponents(double vx, double vy)
{
	m_vector[0] = vx;
	m_vector[1] = vy;
	m_vector[2] = 0.0;
}

void CVec3d::setComponents(const double *v)
{
	m_vector[0] = v[0];
	m_vector[1] = v[1];
	m_vector[2] = v[2];
}

void CVec3d::getComponents(double &vx, double &vy, double &vz)
{
	vx = m_vector[0];
	vy = m_vector[1];
	vz = m_vector[2];
}

CVec3d& CVec3d::operator+=(CVec3d& a)
{
	m_vector[0] += a.m_vector[0];
	m_vector[1] += a.m_vector[1];
	m_vector[2] += a.m_vector[2];
	return *this;
}

/*
CVec3d& CVec3d::operator=(CVec3d& a)
{
	m_vector[0] = a.m_vector[0];
	m_vector[1] = a.m_vector[1];
	m_vector[2] = a.m_vector[2];
	return *this;
}
*/

CVec3d& CVec3d::operator=(CVec3d& a)
{
	m_vector[0] = a.m_vector[0];
	m_vector[1] = a.m_vector[1];
	m_vector[2] = a.m_vector[2];
	return *this;
}

CVec3d& CVec3d::operator-=(CVec3d& a)
{
	m_vector[0] -= a.m_vector[0];
	m_vector[1] -= a.m_vector[1];
	m_vector[2] -= a.m_vector[2];
	return *this;
}

CVec3d& operator+(CVec3d& a, CVec3d& b)
{
	CVec3d& r = rfemGetTempVec3d();
	r = a;
	return r += b;
}

CVec3d& operator-(CVec3d& a, CVec3d& b)
{
	CVec3d& r = rfemGetTempVec3d();
	r = a;
	return r -= b;
}

double CVec3d::operator[](const int idx)
{
	if ((idx>=0)&&(idx<3))
		return m_vector[idx];
	else
		return 0.0;
}

CVec3d& operator*(CVec3d& a, CVec3d& b)
{
	CVec3d& r = rfemGetTempVec3d();

	double c1, c2, c3;

	c1 = a[1] * b[2] - a[2] * b[1];
	c2 = a[2] * b[0] - a[0] * b[2];
	c3 = a[0] * b[1] - a[1] * b[0];
	
	r.setComponents(c1, c2, c3);

	return r;
}

CVec3d& operator*(CVec3d& a, double b)
{
	CVec3d& r = rfemGetTempVec3d();
	r.setComponents(a[0]*b, a[1]*b, a[2]*b);
	return r;
}

CVec3d& operator*(double a, CVec3d& b)
{
	CVec3d& r = rfemGetTempVec3d();
	r.setComponents(b[0]*a, b[1]*a, b[2]*a);
	return r;
}

void CVec3d::getComponents(double *v)
{
	v[0] = m_vector[0];
	v[1] = m_vector[1];
	v[2] = m_vector[2];
}

double CVec3d::length()
{
	return sqrt(pow(m_vector[0],2) + pow(m_vector[1],2) + pow(m_vector[2],2));
}

void CVec3d::normalize()
{
	double quote = 1.0/length();

	m_vector[0] = m_vector[0] * quote;
	m_vector[1] = m_vector[1] * quote;
	m_vector[2] = m_vector[2] * quote;
}


void CVec3d::rotate(CVec3d &axis, double angle)
{
	double cost = cos(angle*2*M_PI/360.0);
	double sint = sin(angle*2*M_PI/360.0);

	double rv[3];

	rv[0] = 
		(cost + (1.0-cost)*pow(axis[0],2))        * m_vector[0] + 
		((1.0-cost)*axis[0]*axis[1]-axis[2]*sint) * m_vector[1] + 
		((1.0-cost)*axis[0]*axis[2]+axis[1]*sint) * m_vector[2];

	rv[1] = 
		((1.0-cost)*axis[0]*axis[1]+axis[2]*sint) * m_vector[0] +
		(cost + (1.0-cost)*pow(axis[1],2))        * m_vector[1] + 
		((1.0-cost)*axis[1]*axis[2]-axis[0]*sint) * m_vector[2];

	rv[2] = 
		((1.0-cost)*axis[0]*axis[2]-axis[1]*sint) * m_vector[0] + 
		((1.0-cost)*axis[1]*axis[2]+axis[0]*sint) * m_vector[1] +
		(cost+(1.0-cost)*pow(axis[2],2))          * m_vector[2];

	m_vector[0] = rv[0];
	m_vector[1] = rv[1];
	m_vector[2] = rv[2];
}

void CVec3d::getEulerAngles(double &pitch, double &heading)
{
	heading = atan2(m_vector[0], m_vector[2])*180.0/M_PI;
	double t = sqrt(pow(m_vector[0],2)+pow(m_vector[2],2));
	pitch = atan2(m_vector[1], t)*180.0/M_PI;
}

void CVec3d::negate()
{
	m_vector[0] = - m_vector[0];
	m_vector[1] = - m_vector[1];
	m_vector[2] = - m_vector[2];
}


void CVec3d::setX(double value)
{
	m_vector[0] = value;
}

void CVec3d::setY(double value)
{
	m_vector[1] = value;
}

void CVec3d::setZ(double value)
{
	m_vector[2] = value;
}

void CVec3d::print(std::ostream &out)
{
	out << "Vec3d = [ " << m_vector[0] << " " << m_vector[1] << " " << m_vector[2] << " ]" << std::endl;
}

void CVec3d::setFromPoints(CVec3d &pos, CVec3d &target)
{
	CVec3d r;
	r = target - pos;
	r.getComponents(m_vector);
}

void CVec3d::add(double dx, double dy, double dz)
{
	m_vector[0] += dx;
	m_vector[1] += dy;
	m_vector[2] += dz;
}

double* CVec3d::getComponents()
{
	return &m_vector[0];
}

double CVec3d::getX()
{
	return m_vector[0];
}	

double CVec3d::getY()
{
	return m_vector[1];
}

double CVec3d::getZ()
{
	return m_vector[2];
}
