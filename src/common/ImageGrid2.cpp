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

#ifdef __APPLE__
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

CImageGrid2::CImageGrid2()
{
	m_fieldLayers = 2;
	m_image = nullptr;
	m_stride = 8;
	m_rows = -1;
	m_cols = -1;
	m_maxIntensity = 1.0;
	m_useImage = true;
	m_averageOverGridSquare = true;
	m_elementScaleFactor = 1.0;
}

CImageGrid2::~CImageGrid2()
{
	this->clearGrid();
}

void CImageGrid2::clearGrid()
{
	m_grid.resize(0, 0);
	m_fields.clear();
	m_specialElement.clear();
}

void CImageGrid2::setImage(CImagePtr image)
{
	m_image = image;

	m_width = m_image->getWidth();
	m_height = m_image->getHeight();
}

void CImageGrid2::doGeometry()
{
	int i, j;

	if (m_grid.size() > 0)
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
				if (m_grid(i,j)>0.0)
				{
					glBegin(GL_TRIANGLES);
					glColor3f((1.0-m_grid(i,j))*m_maxIntensity, (1.0-m_grid(i,j))*m_maxIntensity, (1.0-m_grid(i,j))*m_maxIntensity);
					glVertex2i(j*m_stride, i*m_stride);
					glVertex2i((j+1)*m_stride, i*m_stride);
					glVertex2i((j+1)*m_stride, (i+1)*m_stride);
					glEnd();

					glBegin(GL_TRIANGLES);
					glColor3f((1.0-m_grid(i,j))*m_maxIntensity, (1.0-m_grid(i,j))*m_maxIntensity, (1.0-m_grid(i,j))*m_maxIntensity);
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

	m_grid = EigenGridMatrix::Zero(m_rows, m_cols);
	m_specialElement.assign(m_rows * m_cols, 0);
	m_fields.assign(m_fieldLayers, EigenGridMatrix::Zero(m_rows, m_cols));

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

				m_grid(i,j) = 1.0 - gridSum1/(double)nGridValues1/255.0f;

				if (m_grid(i,j)>0.0)
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
	return m_image.get();
}

int CImageGrid2::getStride()
{
	return m_stride;
}

double CImageGrid2::getGridValue(int row, int col)
{
	if (m_grid.size() > 0)
	{
		if ((row>=0)&&(row<m_rows)&&(col>=0)&&(col<m_cols))
			return m_grid(row, col);
		else
			return 0.0;
	}
	else
		return 0.0;
}

void CImageGrid2::setFieldValue(int layer, int row, int col, double value)
{
	if (!m_fields.empty())
		if ((row>=0)&&(row<m_rows)&&(col>=0)&&(col<m_cols)&&(layer>=0)&&(layer<m_fieldLayers))
			m_fields[layer](row, col) = value;
}

double CImageGrid2::getFieldValue(int layer, int row, int col)
{
	if (!m_fields.empty())
	{
		if ((row>=0)&&(row<m_rows)&&(col>=0)&&(col<m_cols)&&(layer>=0)&&(layer<m_fieldLayers))
			return m_fields[layer](row, col);
		else
			return 0.0;
	}
	else
		return 0.0;
}

void CImageGrid2::copyField(int fromLayer, int toLayer)
{
	if (!m_fields.empty())
		if ((fromLayer>=0)&&(fromLayer<m_fieldLayers)&&(toLayer>=0)&&(toLayer<m_fieldLayers))
			m_fields[toLayer] = m_fields[fromLayer];
}

void CImageGrid2::copyField(int fromLayer, Eigen::MatrixXd& toMatrix)
{
	if (!m_fields.empty())
		if ((fromLayer>=0)&&(fromLayer<m_fieldLayers))
			toMatrix = m_fields[fromLayer];
}

void CImageGrid2::copyFromGrid(int toLayer, double factor)
{
	if (!m_fields.empty() && m_grid.size() > 0)
		if ((toLayer>=0)&&(toLayer<m_fieldLayers))
			m_fields[toLayer] = m_grid * factor;
}

double CImageGrid2::maxAbsDiff(int l1, int l2)
{
	if (!m_fields.empty())
		if ((l1>=0)&&(l1<m_fieldLayers)&&(l2>=0)&&(l2<m_fieldLayers))
			return (m_fields[l1] - m_fields[l2]).cwiseAbs().maxCoeff();
	return 0.0;
}

void CImageGrid2::assignField(int layer, double value)
{
	if (!m_fields.empty() && (layer>=0) && (layer<m_fieldLayers))
		m_fields[layer].fill(value);
}

void CImageGrid2::assignField(int layer, Eigen::MatrixXd& toMatrix)
{
	if (!m_fields.empty())
		if ((layer>=0)&&(layer<m_fieldLayers)&&(toMatrix.rows()==m_rows)&&(toMatrix.cols()==m_cols))
			m_fields[layer] = toMatrix;
}

void CImageGrid2::copyGrid(Eigen::MatrixXd& toMatrix)
{
	if (m_grid.size() > 0)
		toMatrix = m_grid;
}

void CImageGrid2::copyGrid(Eigen::MatrixXd& toMatrix, double scaleFactor)
{
	if (m_grid.size() > 0)
		toMatrix = m_grid * scaleFactor;
}

void CImageGrid2::assignGrid(Eigen::MatrixXd& toMatrix)
{
	if (m_grid.size() > 0 && toMatrix.rows()==m_rows && toMatrix.cols()==m_cols)
		m_grid = toMatrix;
}

void CImageGrid2::assignFieldFromImage(int imageLayer, int toFieldLayer)
{
	if (m_useImage)
	{

		if ((imageLayer>=m_image->getLayerCount())||(imageLayer<0))
			return;

		if ((toFieldLayer>=m_fieldLayers)||(toFieldLayer<0))
			return;

		int prevLayer = m_image->getLayer();
		m_image->setLayer(imageLayer);

		int i, j, k, l;
		double gridSum1;
		double nGridValues1;
		GLubyte red, green, blue;

		int nNonZeroValues = 0;

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
						red = 0; green = 0; blue = 0;
						if ((i*m_stride+k<m_height)&&(j*m_stride+l<m_width))
							m_image->getPixel(j*m_stride+l, i*m_stride+k, red, green, blue);
							//m_image->getValue(j*m_stride+l, i*m_stride+k, 0, gridValue);

						gridSum1 += (double)red;
						gridSum1 += (double)green;
						gridSum1 += (double)blue;
					}
				}

				m_fields[toFieldLayer](i, j) = 1.0 - gridSum1/(double)nGridValues1/255.0f/3.0f;

				if (m_fields[toFieldLayer](i, j)>0.0)
					nNonZeroValues++;
			}
		}

		m_image->setLayer(prevLayer);
	}
}

bool CImageGrid2::isSpecialElement(int row, int col)
{
	if (!m_specialElement.empty())
	{
		if ((row>=0)&&(row<m_rows)&&(col>=0)&&(col<m_cols))
			return m_specialElement[row * m_cols + col] != 0;
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

	if (m_grid.size() > 0)
	{
		ex[0] = (double)col*m_stride*m_elementScaleFactor;
		ex[1] = (double)(col+1)*m_stride*m_elementScaleFactor;
		ex[2] = (double)(col+1)*m_stride*m_elementScaleFactor;
		ex[3] = (double)col*m_stride*m_elementScaleFactor;

		ey[0] = (double)row*m_stride*m_elementScaleFactor;
		ey[1] = (double)row*m_stride*m_elementScaleFactor;
		ey[2] = (double)(row+1)*m_stride*m_elementScaleFactor;
		ey[3] = (double)(row+1)*m_stride*m_elementScaleFactor;
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

	if (m_grid.size() > 0)
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
	if (m_grid.size() > 0)
		if ((row>=0)&&(row<m_rows)&&(col>=0)&&(col<m_cols))
			m_grid(row, col) = value;
}

void CImageGrid2::setSpecialElement(int row, int col, bool special)
{
	if (!m_specialElement.empty())
		if ((row>=0)&&(row<m_rows)&&(col>=0)&&(col<m_cols))
			m_specialElement[row * m_cols + col] = special ? 1 : 0;
}

void CImageGrid2::setImageSize(int width, int height)
{
	m_width = width;
	m_height = height;
}

void CImageGrid2::setElementScaleFactor(double factor)
{
	m_elementScaleFactor = factor;
}
