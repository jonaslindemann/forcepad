//
// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2007 Division of Structural Mecahnics, Lund University
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

#include "ImageGrid.h"

CImageGrid::CImageGrid()
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

CImageGrid::~CImageGrid()
{
	int i, j;

	// Delete old grid, if any.

	if (m_grid!=NULL)
	{
		for (i=0; i<m_rows; i++)
		{
			for (j=0; j<m_cols; j++)
			{
				delete [] m_grid[i][j];
				delete [] m_specialElement[i][j];
			}

			delete [] m_grid[i];
			delete [] m_specialElement[i];
		}

		delete [] m_grid;
		delete [] m_specialElement;
	}
}

void CImageGrid::setImage(CImage *image)
{
	m_image = image;

	m_width = m_image->getWidth();
	m_height = m_image->getHeight();
}

void CImageGrid::doGeometry()
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
				if (((i+j)%2)==0)
				{
					if (m_grid[i][j][0]>0.0)
					{
						glBegin(GL_TRIANGLES);
						glColor3f((1.0-m_grid[i][j][0])*m_maxIntensity, (1.0-m_grid[i][j][0])*m_maxIntensity, (1.0-m_grid[i][j][0])*m_maxIntensity);
						glVertex2i(j*m_stride, i*m_stride);
						glVertex2i((j+1)*m_stride, i*m_stride);
						glVertex2i((j+1)*m_stride, (i+1)*m_stride);
						glEnd();

						glBegin(GL_TRIANGLES);
						glColor3f((1.0-m_grid[i][j][1])*m_maxIntensity, (1.0-m_grid[i][j][1])*m_maxIntensity, (1.0-m_grid[i][j][1])*m_maxIntensity);
						glVertex2i(j*m_stride, i*m_stride);
						glVertex2i((j+1)*m_stride, (i+1)*m_stride);
						glVertex2i(j*m_stride, (i+1)*m_stride);
						glEnd();
					}
				}
				else
				{
					if (m_grid[i][j][0]>0.0)
					{
						glBegin(GL_TRIANGLES);
						glColor3f((1.0-m_grid[i][j][0])*m_maxIntensity, (1.0-m_grid[i][j][0])*m_maxIntensity, (1.0-m_grid[i][j][0])*m_maxIntensity);
						glVertex2i(j*m_stride, i*m_stride);
						glVertex2i((j+1)*m_stride, i*m_stride);
						glVertex2i(j*m_stride, (i+1)*m_stride);
						glEnd();

						glBegin(GL_TRIANGLES);
						glColor3f((1.0-m_grid[i][j][1])*m_maxIntensity, (1.0-m_grid[i][j][1])*m_maxIntensity, (1.0-m_grid[i][j][1])*m_maxIntensity);
						glVertex2i((j+1)*m_stride, i*m_stride);
						glVertex2i((j+1)*m_stride, (i+1)*m_stride);
						glVertex2i(j*m_stride, (i+1)*m_stride);
						glEnd();
					}
				}
			}
		}
	}
}

void CImageGrid::setStride(int stride)
{
	m_stride = stride;
}

void CImageGrid::initGrid()
{
	int i, j, k, l;
	double gridSum1, gridSum2, gridSumDiag;
	GLubyte gridValue;
	int nGridValues1, nGridValues2, nGridValuesDiag;
	int nNonZeroValues;

	// Delete old grid, if any.

	if (m_grid!=NULL)
	{
		for (i=0; i<m_rows; i++)
		{
			for (j=0; j<m_cols; j++)
			{
				delete [] m_grid[i][j];
				delete [] m_specialElement[i][j];
			}

			delete [] m_grid[i];
			delete [] m_specialElement[i];
		}

		delete [] m_grid;
		delete [] m_specialElement;
	}

	// Create new grid

	if (m_useImage)
	{
		m_rows = m_height/m_stride;
		m_cols = m_width/m_stride;
	}

	m_grid = new double** [m_rows];
	m_specialElement = new bool** [m_rows];

	for (i=0; i<m_rows; i++)
	{
		m_grid[i] = new double* [m_cols];
		m_specialElement[i] = new bool* [m_cols];
		for (j=0; j<m_cols; j++)
		{
			m_grid[i][j] = new double[2];
			m_grid[i][j][0] = 0.0;
			m_grid[i][j][1] = 0.0;
			m_specialElement[i][j] = new bool[2];
			m_specialElement[i][j][0] = false;
			m_specialElement[i][j][1] = false;
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

				if (m_averageOverGridSquare)
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

					m_grid[i][j][0] = 1.0 - gridSum1/(double)nGridValues1/255.0f;
					m_grid[i][j][1] = 1.0 - gridSum1/(double)nGridValues1/255.0f;

					if ((m_grid[i][j][0]>0.0)||(m_grid[i][j][1]>0.0))
						nNonZeroValues++;
				}
				else
				{

					// Determine grid cell type

					if (((i+j)%2)==0)
					{

						// Calculate average of lower right half

						gridSum1 = 0.0;
						nGridValues1 = 0;
						for (k=0; k<m_stride; k++)
						{
							for (l=k+1; l<m_stride; l++)
							{
								nGridValues1++;
								if ((i*m_stride+k<m_height)&&(j*m_stride+l<m_width))
									m_image->getValue(j*m_stride+l, i*m_stride+k, 0, gridValue);
								else
									gridValue = 0.0;
								gridSum1 += (double)gridValue;
							}
						}

						// Calculate average of upper left half

						gridSum2 = 0.0;
						nGridValues2 = 0;
						for (k=1; k<m_stride; k++)
						{
							for (l=0; l<m_stride-k; l++)
							{
								nGridValues2++;
								if ((i*m_stride+k<m_height)&&(j*m_stride+l<m_width))
									m_image->getValue(j*m_stride+l, i*m_stride+k, 0, gridValue);
								else
									gridValue = 0.0;
								gridSum2 += (double)gridValue;
							}
						}

						// Adjust for diagonal

						gridSumDiag = 0.0;
						nGridValuesDiag = 0;
						for (k=0; k<m_stride; k++)
						{
							l = k;

							nGridValuesDiag++;
							if ((i*m_stride+k<m_height)&&(j*m_stride+l<m_width))
								m_image->getValue(j*m_stride+l, i*m_stride+k, 0, gridValue);
							else
								gridValue = 0.0;
							gridSumDiag += (double)gridValue;
						}
					}
					else
					{

						// Calculate average of lower left half

						gridSum1 = 0.0;
						nGridValues1 = 0;
						for (k=0; k<m_stride; k++)
						{
							for (l=0; l<m_stride-k-1; l++)
							{
								nGridValues1++;
								if ((i*m_stride+k<m_height)&&(j*m_stride+l<m_width))
									m_image->getValue(j*m_stride+l, i*m_stride+k, 0, gridValue);
								else
									gridValue = 0.0;
								gridSum1 += (double)gridValue;
							}
						}

						// Calculate average of upper right half

						gridSum2 = 0.0;
						nGridValues2 = 0;
						for (k=1; k<m_stride; k++)
						{
							for (l=m_stride-k-1; l<m_stride; l++)
							{
								nGridValues2++;
								if ((i*m_stride+k<m_height)&&(j*m_stride+l<m_width))
									m_image->getValue(j*m_stride+l, i*m_stride+k, 0, gridValue);
								else
									gridValue = 0.0;
								gridSum2 += (double)gridValue;
							}
						}

						// Adjust for diagonal

						gridSumDiag = 0.0;
						nGridValuesDiag = 0;
						for (k=0; k<m_stride; k++)
						{
							l = m_stride-k-1;
							nGridValuesDiag++;
							if ((i*m_stride+k<m_height)&&(j*m_stride+l<m_width))
								m_image->getValue(j*m_stride+l, i*m_stride+k, 0, gridValue);
							else
								gridValue = 0;
							gridSumDiag += (double)gridValue;
						}
					}
					m_grid[i][j][0] = 1.0 - (gridSum1+gridSumDiag/2.0f)/(double)(nGridValues1+nGridValuesDiag/2)/255.0f;
					m_grid[i][j][1] = 1.0 - (gridSum2+gridSumDiag/2.0f)/(double)(nGridValues2+nGridValuesDiag/2)/255.0f;

					if ((m_grid[i][j][0]>0.0)||(m_grid[i][j][1]>0.0))
						nNonZeroValues++;
				}
			}
		}
	}
}

void CImageGrid::getGridSize(int &rows, int &cols)
{
	rows = m_rows;
	cols = m_cols;
}

CImage* CImageGrid::getImage()
{
	return m_image;
}

int CImageGrid::getStride()
{
	return m_stride;
}

double CImageGrid::getGridValue(int row, int col, int element)
{
	if (m_grid!=NULL)
	{
		if ((row>=0)&&(row<m_rows)&&(col>=0)&&(col<m_cols)&&(element>=0)&&(element<2))
			return m_grid[row][col][element];
		else
			return 0.0;
	}
	else
		return 0.0;
}

bool CImageGrid::isSpecialElement(int row, int col, int element)
{
	if (m_specialElement!=NULL)
	{
		if ((row>=0)&&(row<m_rows)&&(col>=0)&&(col<m_cols)&&(element>=0)&&(element<2))
			return m_specialElement[row][col][element];
		else
			return false;
	}
	else
		return false;
}

CImageGrid::TElementType CImageGrid::getGridElementType(int row, int col)
{
	if (((row+col)%2)==0)
		return CImageGrid::ET_RIGHT_DIAGONAL;
	else
		return CImageGrid::ET_LEFT_DIAGONAL;
}

void CImageGrid::snapToGrid(int& x, int& y)
{
	int row, col;

	col = (int)((double)x / (double)m_stride + 0.5);
	row = (int)((double)y / (double)m_stride + 0.5);

	x = m_stride * col;
	y = m_stride * row;
}

void CImageGrid::getElementCoords(int row, int col, int element, double *ex, double *ey)
{
	if (m_grid!=NULL)
	{
		switch (getGridElementType(row, col)) {
		case ET_RIGHT_DIAGONAL:
			if (element==0)
			{
				ex[0] = (double)col*m_stride;
				ex[1] = (double)(col+1)*m_stride;
				ex[2] = (double)(col+1)*m_stride;

				ey[0] = (double)row*m_stride;
				ey[1] = (double)row*m_stride;
				ey[2] = (double)(row+1)*m_stride;
			}
			else
			{
				ex[0] = (double)col*m_stride;
				ex[1] = (double)(col+1)*m_stride;
				ex[2] = (double)col*m_stride;

				ey[0] = (double)row*m_stride;
				ey[1] = (double)(row+1)*m_stride;
				ey[2] = (double)(row+1)*m_stride;
			}
			break;
		case ET_LEFT_DIAGONAL:
			if (element==0)
			{
				ex[0] = (double)col*m_stride;
				ex[1] = (double)(col+1)*m_stride;
				ex[2] = (double)col*m_stride;

				ey[0] = (double)row*m_stride;
				ey[1] = (double)row*m_stride;
				ey[2] = (double)(row+1)*m_stride;
			}
			else
			{
				ex[0] = (double)(col+1)*m_stride;
				ex[1] = (double)(col+1)*m_stride;
				ex[2] = (double)col*m_stride;

				ey[0] = (double)row*m_stride;
				ey[1] = (double)(row+1)*m_stride;
				ey[2] = (double)(row+1)*m_stride;
			}
			break;
		default:

			break;
		}
	}
}

void CImageGrid::getElementTopo(int row, int col, int element, int *dx, int *dy)
{
	//
	//     ET_RIGHT_DIAGONAL
	//
	//     e = element
	//
	//     o----------o
	//     |        / |
	//     | e=1  /   |
	//     |    /     |
	//     |  /   e=0 |
	//     |/         |
	//     o----------o
	//
	//     e=0: dx = [0, 1, 1]
	//          dy = [0, 0, 1]
	//
	//     e=1: dx = [0, 1, 1]
	//          dy = [0, 0, 1]
	//
	//
	//     ET_LEFT_DIAGONAL
	//
	//     e = element
	//
	//     o----------o
	//     |\         |
	//     |  \  e=1  |
	//     |    \     |
	//     | e=0  \   |
	//     |        \ |
	//     o----------o
	//
	//     e=0: dx = [0, 1, 0]
	//          dy = [0, 0, 1]
	//
	//     e=1: dx = [1, 1, 0]
	//          dy = [0, 1, 1]
	//
	if (m_grid!=NULL)
	{
		switch (getGridElementType(row, col)) {
		case ET_RIGHT_DIAGONAL:
			if (element==0)
			{
				dx[0] = 0;
				dx[1] = 1;
				dx[2] = 1;

				dy[0] = 0;
				dy[1] = 0;
				dy[2] = 1;
			}
			else
			{
				dx[0] = 0;
				dx[1] = 1;
				dx[2] = 0;

				dy[0] = 0;
				dy[1] = 1;
				dy[2] = 1;
			}
			break;
		case ET_LEFT_DIAGONAL:
			if (element==0)
			{
				dx[0] = 0;
				dx[1] = 1;
				dx[2] = 0;

				dy[0] = 0;
				dy[1] = 0;
				dy[2] = 1;
			}
			else
			{
				dx[0] = 1;
				dx[1] = 1;
				dx[2] = 0;

				dy[0] = 0;
				dy[1] = 1;
				dy[2] = 1;
			}
			break;
		default:

			break;
		}
	}
}

void CImageGrid::setMaxIntensity(double maxIntensity)
{
	m_maxIntensity = maxIntensity;
}

void CImageGrid::setGridSize(int rows, int cols)
{
	m_rows = rows;
	m_cols = cols;
}

void CImageGrid::setUseImage(bool flag)
{
	m_useImage = flag;
}

void CImageGrid::setGridValue(int row, int col, int element, double value)
{
	if (m_grid!=NULL)
	{
		if ((row>=0)&&(row<m_rows)&&(col>=0)&&(col<m_cols)&&(element>=0)&&(element<2))
			m_grid[row][col][element] = value;
	}
}

void CImageGrid::setSpecialElement(int row, int col, int element, bool special)
{
	if (m_specialElement!=NULL)
	{
		if ((row>=0)&&(row<m_rows)&&(col>=0)&&(col<m_cols)&&(element>=0)&&(element<2))
			m_specialElement[row][col][element] = special;
	}
}

void CImageGrid::setImageSize(int width, int height)
{
	m_width = width;
	m_height = height;
}
