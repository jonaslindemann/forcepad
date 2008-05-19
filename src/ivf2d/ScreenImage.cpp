//
// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2006 Division of Structural Mecahnics, Lund University
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

#include "ScreenImage.h"

CScreenImage::CScreenImage()
{
	setUseRasterPosition(true);
	m_subImagePos[0] = 0;
	m_subImagePos[1] = 0;
	m_subImageSize[0] = -1;
	m_subImageSize[1] = -1;
	m_rows = 10;
	m_cols = 12;
	m_tileSpacing[0] = -1;
	m_tileSpacing[1] = -1;
	m_renderMode = RM_NORMAL;
}

CScreenImage::~CScreenImage()
{

}

void CScreenImage::doGeometry()
{
	if (m_image!=NULL)
	{
		int i, j;

		switch (m_renderMode) {
		case RM_SUBIMAGE:
			glPixelStorei(GL_UNPACK_ROW_LENGTH, m_image->getWidth());
			glPixelStorei(GL_UNPACK_SKIP_PIXELS, m_subImagePos[0]);
			glPixelStorei(GL_UNPACK_SKIP_ROWS, m_subImagePos[1]);
			glDrawPixels(m_subImageSize[0], m_subImageSize[1], GL_RGBA, GL_UNSIGNED_BYTE, m_image->getImageMap());
			glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
			glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
			glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
			break;
		case RM_NORMAL:
			glDrawPixels(m_image->getWidth(), m_image->getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, m_image->getImageMap());
			break;
		case RM_TILED:
			double x, y;
			this->getPosition(x, y);
			for (i=0; i<m_rows; i++)
			{
				for (j=0; j<m_cols; j++)
				{
					glPixelStorei(GL_UNPACK_ROW_LENGTH, m_image->getWidth());
					glPixelStorei(GL_UNPACK_SKIP_PIXELS, j*m_tileSpacing[0]);
					glPixelStorei(GL_UNPACK_SKIP_ROWS, i*m_tileSpacing[1]);
					glRasterPos2i((int)x + j*m_tileSpacing[0], (int)y + i*m_tileSpacing[1]);
					glDrawPixels(m_tileSpacing[0], m_tileSpacing[1], GL_RGBA, GL_UNSIGNED_BYTE, m_image->getImageMap());
					glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
					glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
					glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
				}
			}
			this->setPosition(x, y);
			break;
		}
	}
}

void CScreenImage::setImage(CImage *image)
{
	m_image = image;
	m_subImageSize[0] = m_image->getWidth();
	m_subImageSize[1] = m_image->getHeight();
	m_tileSpacing[0] = m_image->getWidth() / m_cols;
	m_tileSpacing[1] = m_image->getHeight() / m_rows;
}

void CScreenImage::setSubImageSize(int width, int height)
{
	m_subImageSize[0] = width;
	m_subImageSize[1] = height;
}

void CScreenImage::getSubImageSize(int &width, int &height)
{
	width = m_subImageSize[0];
	height = m_subImageSize[1];
}

void CScreenImage::reset()
{
	if (m_image!=NULL)
	{
		m_subImagePos[0] = 0;
		m_subImagePos[1] = 1;
		m_subImageSize[0] = m_image->getWidth();
		m_subImageSize[1] = m_image->getHeight();
	}
}


void CScreenImage::setRenderMode(TRenderMode mode)
{
	m_renderMode = mode;
}

CScreenImage::TRenderMode CScreenImage::getRenderMode()
{
	return m_renderMode;
}

void CScreenImage::setTiles(int rows, int cols)
{
	m_rows = rows;
	m_cols = cols;

	if (m_image!=NULL)
	{
		m_tileSpacing[0] = m_image->getWidth() / m_cols;
		m_tileSpacing[1] = m_image->getHeight() / m_rows;
	}
}

void CScreenImage::getTiles(int &rows, int &cols)
{
	rows = m_rows;
	cols = m_cols;
}

int CScreenImage::getRows()
{
	return m_rows;
}

int CScreenImage::getCols()
{
	return m_cols;
}

void CScreenImage::update(int x1, int y1, int x2, int y2)
{
	if (m_image!=NULL)
	{
		int xmin, ymin, xmax, ymax;

		if (x2>x1)
		{
			xmin = x1;
			xmax = x2;
		}
		else
		{
			xmin = x2;
			xmax = x1;
		}

		if (y2>y1)
		{
			ymin = y1;
			ymax = y2;
		}
		else
		{
			ymin = y2;
			ymax = y1;
		}

		double x, y;
		this->getPosition(x, y);

		if (xmax>m_image->getWidth())
			xmax = m_image->getWidth();
		if (ymax>m_image->getHeight())
			ymax = m_image->getHeight();
		if (ymin<0)
			ymin = 0;
		if (xmin<0)
			xmin = 0;
	
		glPixelStorei(GL_PACK_ROW_LENGTH, m_image->getWidth());
		glPixelStorei(GL_PACK_SKIP_PIXELS, xmin);
		glPixelStorei(GL_PACK_SKIP_ROWS, ymin);
		glReadPixels((int)x + xmin, (int)y + ymin, xmax-xmin, ymax-ymin, GL_RGBA, GL_UNSIGNED_BYTE, m_image->getImageMap());
		glPixelStorei(GL_PACK_ROW_LENGTH, 0);
		glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
		glPixelStorei(GL_PACK_SKIP_ROWS, 0);

		m_image->fillRectAlpha(x1, y1, x2, y2, 128);
	}
}

