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

#ifndef _CImageGrid_h_
#define _CImageGrid_h_

#include "Image.h"
#include "Shape.h"

IvfSmartPointer(CImageGrid);

class CImageGrid : public CShape {
private:
	CImagePtr m_image;
	double m_maxIntensity;
	bool m_useImage;
	bool m_averageOverGridSquare;
protected:
	double*** m_grid;
	bool*** m_specialElement;
	int m_width;
	int m_height;
	int m_rows;
	int m_cols;
	int m_stride;
public:
	enum TElementType { ET_RIGHT_DIAGONAL, ET_LEFT_DIAGONAL };
public:
	void setImageSize(int width, int height);
	CImageGrid();
	virtual ~CImageGrid();

	IvfClassInfo("CImageGrid",CShape);

	virtual void initGrid();

	virtual void setImage(CImage* image);
	CImage* getImage();

	void setStride(int stride);
	int getStride();

	void setGridSize(int rows, int cols);
	void getGridSize(int &rows, int &cols);

	TElementType getGridElementType(int row, int col);
	
	void setGridValue(int row, int col, int element, double value);
	double getGridValue(int row, int col, int element);

	void getElementTopo(int row, int col, int element, int* dx, int* dy);
	void getElementCoords(int row, int col, int element, double* ex, double* ey);

	void setUseImage(bool flag);

	void setMaxIntensity(double maxIntensity);

	void snapToGrid(int& x, int& y);

	bool isSpecialElement(int row, int col, int element);
	void setSpecialElement(int row, int col, int element, bool special);

	virtual void doGeometry();
};

#endif 
