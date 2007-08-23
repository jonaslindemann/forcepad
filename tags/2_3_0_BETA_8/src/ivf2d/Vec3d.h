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

#ifndef _CVec3d_h_
#define _CVec3d_h_

/**
 * Simple 3 component vector class
 *
 * CVec3d implements a 3 component vector with standard
 * operators and methods. It should also be used instead
 * of the cold IvfVector class. 
 * @author Jonas Lindemann
 */
class CVec3d {
private:
	double m_vector[3];
public:
	double getZ();
	double getY();
	double getX();
	/** Class constructor. */
	CVec3d();

	/** Class constructor. */
	CVec3d(double vx, double vy, double vz);

	/** Class assignment constructor. */
	CVec3d(const CVec3d& v);

	/** Class destructor. */
	virtual ~CVec3d();

	/** 
	 * Rotate vector around an axis. 
	 *
	 * @param axis is the rotation axis
	 * @param angle is the amount of rotation around the axis.
	 */
	void rotate(CVec3d& axis, double angle);

	/** Normalizes the vector. Length = 1.0.*/
	void normalize();

	/** Returns the vector length. */
	double length();

	/** Negate vector. */
	void negate();

	/** Add to vector. */
	void add(double dx, double dy, double dz);

	/** 
	 * Set vector from to points/vectors. 
	 * 
	 * @param pos is the starting point of the vector.
	 * @param target is the endpoint of the vector.
	 */
	void setFromPoints(CVec3d& pos, CVec3d& target);

	/** Return a pointer to vector components */
	double* getComponents();

	/** Set components of vector. */
	void setComponents(double vx, double vy, double vz);
	/** Set components of vector. */
	void setComponents(const double *v);
	/** Get vector components. */
	void getComponents(double &vx, double &vy, double &vz);
	/** Get vector components. */
	void getComponents(double* v);

	/** Calculate euler angles of vector. */
	void getEulerAngles(double &pitch, double &heading);


	/** Set z-component of vector */
	void setZ(double value);
	/** Set y-component of vector */
	void setY(double value);
	/** Set x-component of vector */
	void setX(double value);

	/** Vector addition operator. */
	CVec3d& operator+=(CVec3d a);
	/** Vector subtraction operator. */
	CVec3d& operator-=(CVec3d a);
	/** Assignment operator. */
	CVec3d& operator=(CVec3d a);
	/** Index operator. */
	double operator[](const int idx);

};

/** Vector crossproduct operator. */
CVec3d operator*(CVec3d a, CVec3d b);

/** Vector scalar multiplication operator. */
CVec3d operator*(CVec3d a, double b); 

/** Vector scalar multiplication operator. */
CVec3d operator*(double a, CVec3d b);

/** Vector addition operator. */
CVec3d operator+(CVec3d a, CVec3d b);

/** Vector subtraction operator. */
CVec3d operator-(CVec3d a, CVec3d b);

#endif 
