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

#include "ImageGrid2.h"

CImageGrid2::CImageGrid2()
{
	m_image = NULL;
	m_stride = 8;
	m_grid = NULL;
	m_specialElement = NULL;
	m_rows = -1;
	m_cols = -1;
	m_maxIntensity = 1.0;
	m_useImage = true;
	m_averageOverGridSquare = true;
}

CImageGrid2::~CImageGrid2()
{
	this->clearGrid();
}

void CImageGrid2::clearGrid()
{
	int i;

	// Delete old grid, if any.

	if (m_grid!=NULL)
	{
		for (i=0; i<m_rows; i++)
		{
			delete [] m_grid[i];
			delete [] m_specialElement[i];
		}

		delete [] m_grid;
		delete [] m_specialElement;
	}
}

void CImageGrid2::setImage(CImage *image)
{
	m_image = image;

	m_width = m_image->getWidth();
	m_height = m_image->getHeight();
}

void CImageGrid2::doGeometry()
{
	int i, j;

	if (m_grid!=NULL)
	{
		/*
		glBegin(GL_LINES);
		glColor3f(1.0, 0.0, 0.0);
		glVertex2i(0,0);
		glVertex2i(50,50);
		glEnd();
		*/
		for (i=0; i<m_rows; i++)
		{
			for (j=0; j<m_cols; j++)
			{
				if (m_grid[i][j]>0.0)
				{
					glBegin(GL_TRIANGLES);
					glColor3f((1.0-m_grid[i][j])*m_maxIntensity, (1.0-m_grid[i][j])*m_maxIntensity, (1.0-m_grid[i][j])*m_maxIntensity);
					glVertex2i(j*m_stride, i*m_stride);
					glVertex2i((j+1)*m_stride, i*m_stride);
					glVertex2i((j+1)*m_stride, (i+1)*m_stride);
					glEnd();

					glBegin(GL_TRIANGLES);
					glColor3f((1.0-m_grid[i][j])*m_maxIntensity, (1.0-m_grid[i][j])*m_maxIntensity, (1.0-m_grid[i][j])*m_maxIntensity);
					glVertex2i(j*m_stride, i*m_stride);
					glVertex2i((j+1)*m_stride, (i+1)*m_stride);
					glVertex2i(j*m_stride, (i+1)*m_stride);
					glEnd();
				}
			}
		}
	}
}

void CImageGrid2::setStride(int stride)
{
	m_stride = stride;
}

void CImageGrid2::initGrid()
{
	int i, j, k, l;
	double gridSum1;
	GLubyte gridValue;
	int nGridValues1;
	int nNonZeroValues;

	// Delete old grid, if any.

	this->clearGrid();

	// Create new grid

	if (m_useImage)
	{
		m_rows = m_height/m_stride;
		m_cols = m_width/m_stride;
	}

	m_grid = new double* [m_rows];
	m_specialElement = new bool* [m_rows];

	for (i=0; i<m_rows; i++)
	{
		m_grid[i] = new double [m_cols];
		m_specialElement[i] = new bool [m_cols];
		for (j=0; j<m_cols; j++)
		{
			m_grid[i][j] = 0.0;
			m_specialElement[i][j] = false;
		}
	}

	// Fill grid

	if (m_useImage)
	{

		nNonZeroValues = 0;

		for (i=0; i<m_rows; i++)
		{
			for (j=0; j<m_cols; j++)
			{
				// Calculate average of grid square

				gridSum1 = 0.0;
				nGridValues1 = 0;
				for (k=0; k<m_stride; k++)
				{
					for (l=0; l<m_stride; l++)
					{
						nGridValues1++;
						if ((i*m_stride+k<m_height)&&(j*m_stride+l<m_width))
							m_image->getValue(j*m_stride+l, i*m_stride+k, 0, gridValue);
						else
							gridValue = 0.0;
						gridSum1 += (double)gridValue;
					}
				}

				m_grid[i][j] = 1.0 - gridSum1/(double)nGridValues1/255.0f;

				if (m_grid[i][j]>0.0)
					nNonZeroValues++;
			}
		}
	}
}

void CImageGrid2::getGridSize(int &rows, int &cols)
{
	rows = m_rows;
	cols = m_cols;
}

CImage* CImageGrid2::getImage()
{
	return m_image;
}

int CImageGrid2::getStride()
{
	return m_stride;
}

double CImageGrid2::getGridValue(int row, int col)
{
	if (m_grid!=NULL)
	{
		if ((row>=0)&&(row<m_rows)&&(col>=0)&&(col<m_cols))
			return m_grid[row][col];
		else
			return 0.0;
	}
	else
		return 0.0;
}

bool CImageGrid2::isSpecialElement(int row, int col)
{
	if (m_specialElement!=NULL)
	{
		if ((row>=0)&&(row<m_rows)&&(col>=0)&&(col<m_cols))
			return m_specialElement[row][col];
		else
			return false;
	}
	else
		return false;
}

void CImageGrid2::snapToGrid(int& x, int& y)
{
	int row, col;

	col = (int)((double)x / (double)m_stride + 0.5);
	row = (int)((double)y / (double)m_stride + 0.5);

	x = m_stride * col;
	y = m_stride * row;
}

void CImageGrid2::getElementCoords(int row, int col, double *ex, double *ey)
{
	//       3          2
	//     o----------o 
	//     |          |
	//     |          |
	//     |          |
	//     |          |
	//     | 0        | 1
	//     o----------o
	//

	if (m_grid!=NULL)
	{
		ex[0] = (double)col*m_stride;
		ex[1] = (double)(col+1)*m_stride;
		ex[2] = (double)(col+1)*m_stride;
		ex[3] = (double)col*m_stride;

		ey[0] = (double)row*m_stride;
		ey[1] = (double)row*m_stride;
		ey[2] = (double)(row+1)*m_stride;
		ey[3] = (double)(row+1)*m_stride;
	}
}

void CImageGrid2::getElementTopo(int row, int col, int *dx, int *dy)
{
	//       3          2
	//     o----------o 
	//     |          |
	//     |          |
	//     |          |
	//     |          |
	//     | 0        | 1
	//     o----------o
	//
	//
	//     dx = [0, 1, 1, 0]
	//     dy = [0, 0, 1, 1]
	//

	if (m_grid!=NULL)
	{
		dx[0] = 0;
		dx[1] = 1;
		dx[2] = 1;
		dx[3] = 0;

		dy[0] = 0;
		dy[1] = 0;
		dy[2] = 1;
		dy[3] = 1;
	}
}

void CImageGrid2::setMaxIntensity(double maxIntensity)
{
	m_maxIntensity = maxIntensity;
}

void CImageGrid2::setGridSize(int rows, int cols)
{
	m_rows = rows;
	m_cols = cols;
}

void CImageGrid2::setUseImage(bool flag)
{
	m_useImage = flag;
}

void CImageGrid2::setGridValue(int row, int col, double value)
{
	if (m_grid!=NULL)
	{
		if ((row>=0)&&(row<m_rows)&&(col>=0)&&(col<m_cols))
			m_grid[row][col] = value;
	}
}

void CImageGrid2::setSpecialElement(int row, int col, bool special)
{
	if (m_specialElement!=NULL)
	{
		if ((row>=0)&&(row<m_rows)&&(col>=0)&&(col<m_cols))
			m_specialElement[row][col] = special;
	}
}

void CImageGrid2::setImageSize(int width, int height)
{
	m_width = width;
	m_height = height;
}
