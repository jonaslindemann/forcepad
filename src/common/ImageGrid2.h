//
// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2008 Division of Structural Mecahnics, Lund University
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

#ifndef _CImageGrid_h_
#define _CImageGrid_h_

#include "Image.h"
#include "Shape.h"

#include <include.h>
#include <newmat.h>
#include <newmatio.h>
#include <newmatap.h>

IvfSmartPointer(CImageGrid2);

class CImageGrid2 : public CShape {
private:
	CImagePtr m_image;
	double m_maxIntensity;
	bool m_useImage;
	bool m_averageOverGridSquare;
protected:
	double** m_grid;
	double*** m_fields;
	bool** m_specialElement;
	int m_width;
	int m_height;
	int m_rows;
	int m_cols;
	int m_stride;
	double m_elementScaleFactor;
	int m_fieldLayers;
public:
	CImageGrid2();
	virtual ~CImageGrid2();

	void clearGrid();

	IvfClassInfo("CImageGrid",CShape);

	virtual void initGrid();

	virtual void setImage(CImage* image);
	CImage* getImage();

	void setImageSize(int width, int height);
	void setStride(int stride);
	int getStride();

	void setGridSize(int rows, int cols);
	void getGridSize(int &rows, int &cols);

	void setGridValue(int row, int col, double value);
	double getGridValue(int row, int col);

	void copyGrid(Matrix& toMatrix);
	void copyGrid(Matrix& toMatrix, double scaleFactor);
	void assignGrid(Matrix& toMatrix);

	void setFieldValue(int layer, int row, int col, double value);
	double getFieldValue(int layer, int row, int col);

	void copyField(int fromLayer, int toLayer);
	void copyFromGrid(int toLayer, double factor);
	void assignField(int layer, double value);
	void assignField(int layer, Matrix& toMatrix);
	void copyField(int fromLayer, Matrix& toMatrix);
	void assignFieldFromImage(int imageLayer, int toLayer);

	double maxAbsDiff(int l1, int l2);

	void getElementTopo(int row, int col, int* dx, int* dy);
	void getElementCoords(int row, int col, double* ex, double* ey);

	void setUseImage(bool flag);

	void setMaxIntensity(double maxIntensity);

	void snapToGrid(int& x, int& y);

	bool isSpecialElement(int row, int col);
	void setSpecialElement(int row, int col, bool special);

	void setElementScaleFactor(double factor);

	virtual void doGeometry();
};

#endif 
