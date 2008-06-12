#ifndef _CVec3d_h_
#define _CVec3d_h_

#include "RFemBase.h"

SmartPointer(CVec3d);

class CVec3d : public CBase {
private:
	double m_vector[3];
public:
	CVec3d();
	CVec3d(double vx, double vy, double vz);
	CVec3d(CVec3d& v);
	virtual ~CVec3d();

	ClassInfo("CVec3d",CBase);

	void rotate(CVec3d& axis, double angle);
	void normalize();
	double length();
	void negate();

	void print(std::ostream &out);

	void setFromPoints(CVec3d& pos, CVec3d& target);
	void add(double dx, double dy, double dz);
	void getEulerAngles(double &pitch, double &heading);
	void setComponents(double vx, double vy, double vz);
	void setComponents(double vx, double vy);
	void getComponents(double &vx, double &vy, double &vz);
	void setComponents(const double *v);
	void getComponents(double* v);
	double* getComponents();
	void setZ(double value);
	void setY(double value);
	void setX(double value);

	double getX();
	double getY();
	double getZ();

	CVec3d& operator+=(CVec3d& a);
	CVec3d& operator-=(CVec3d& a);
	CVec3d& operator=(CVec3d& a);
	double operator[](const int idx);

	bool operator==(CVec3d& a);
	bool operator!=(CVec3d& a);
};

CVec3d& operator*(CVec3d& a, CVec3d& b);
CVec3d& operator*(CVec3d& a, double b); 
CVec3d& operator*(double a, CVec3d& b);
CVec3d& operator+(CVec3d& a, CVec3d& b);
CVec3d& operator-(CVec3d& a, CVec3d& b);

CVec3d& rfemGetTempVec3d();

/** \example math.cpp
 * This is an example of how to use some of the math classes.
 */

#endif 
