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

#include <algorithm>
#include <cmath>
#include <vector>

#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>

#include "Renderer2D.h"
#include "StreamTexture.h"

namespace ivf2d {

static QOpenGLExtraFunctions *screenImageGL()
{
	QOpenGLContext *ctx = QOpenGLContext::currentContext();
	return ctx ? ctx->extraFunctions() : nullptr;
}

ScreenImage::ScreenImage()
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
	m_devicePixelRatio = 1.0;
}

void ScreenImage::setDevicePixelRatio(double dpr)
{
    m_devicePixelRatio = dpr > 0.0 ? dpr : 1.0;
}

ScreenImage::~ScreenImage()
{

}

void ScreenImage::doGeometry()
{
	if (m_image == nullptr)
		return;

	const int imgW = m_image->getWidth();
	const int imgH = m_image->getHeight();
	if (imgW <= 0 || imgH <= 0)
		return;

	if (!m_streamTexture)
		m_streamTexture = std::make_unique<StreamTexture>();

	double x, y;
	this->getPosition(x, y);

	Renderer2D &r = Renderer2D::instance();
	r.loadIdentity();
	// The image alpha channel encodes layer markers, not opacity, so the blit
	// must be forced opaque - see Renderer2D::setForceOpaque().
	r.setForceOpaque(true);

	// A single texture holds the whole image; sub-region and tiled modes select
	// portions of it through texture coordinates instead of glDrawPixels
	// unpack-skip state. The quad is drawn with row 0 of the image at the bottom,
	// matching the previous glDrawPixels/glRasterPos orientation.
	m_streamTexture->update(m_image->getImageMap(), imgW, imgH, GL_RGBA);

	switch (m_renderMode) {
	case RM_NORMAL:
		m_streamTexture->draw((float)x, (float)y, (float)imgW, (float)imgH);
		break;

	case RM_SUBIMAGE:
	{
		const float u0 = (float)m_subImagePos[0] / (float)imgW;
		const float v0 = (float)m_subImagePos[1] / (float)imgH;
		const float u1 = (float)(m_subImagePos[0] + m_subImageSize[0]) / (float)imgW;
		const float v1 = (float)(m_subImagePos[1] + m_subImageSize[1]) / (float)imgH;
		r.drawTexturedQuad(m_streamTexture->id(), (float)x, (float)y,
		                   (float)m_subImageSize[0], (float)m_subImageSize[1],
		                   u0, v0, u1, v1);
		break;
	}

	case RM_TILED:
		for (int i = 0; i < m_rows; i++)
		{
			for (int j = 0; j < m_cols; j++)
			{
				const int sx = j * m_tileSpacing[0];
				const int sy = i * m_tileSpacing[1];
				const float u0 = (float)sx / (float)imgW;
				const float v0 = (float)sy / (float)imgH;
				const float u1 = (float)(sx + m_tileSpacing[0]) / (float)imgW;
				const float v1 = (float)(sy + m_tileSpacing[1]) / (float)imgH;
				r.drawTexturedQuad(m_streamTexture->id(),
				                   (float)(x + sx), (float)(y + sy),
				                   (float)m_tileSpacing[0], (float)m_tileSpacing[1],
				                   u0, v0, u1, v1);
			}
		}
		break;
	}

	r.setForceOpaque(false);
}

void ScreenImage::setImage(ImagePtr image)
{
	m_image = image;
	m_subImageSize[0] = m_image->getWidth();
	m_subImageSize[1] = m_image->getHeight();
	m_tileSpacing[0] = m_image->getWidth() / m_cols;
	m_tileSpacing[1] = m_image->getHeight() / m_rows;
}

void ScreenImage::setSubImageSize(int width, int height)
{
	m_subImageSize[0] = width;
	m_subImageSize[1] = height;
}

void ScreenImage::getSubImageSize(int &width, int &height)
{
	width = m_subImageSize[0];
	height = m_subImageSize[1];
}

void ScreenImage::reset()
{
	if (m_image!=nullptr)
	{
		m_subImagePos[0] = 0;
		m_subImagePos[1] = 1;
		m_subImageSize[0] = m_image->getWidth();
		m_subImageSize[1] = m_image->getHeight();
	}
}


void ScreenImage::setRenderMode(TRenderMode mode)
{
	m_renderMode = mode;
}

ScreenImage::TRenderMode ScreenImage::getRenderMode()
{
	return m_renderMode;
}

void ScreenImage::setTiles(int rows, int cols)
{
	m_rows = rows;
	m_cols = cols;

	if (m_image!=nullptr)
	{
		m_tileSpacing[0] = m_image->getWidth() / m_cols;
		m_tileSpacing[1] = m_image->getHeight() / m_rows;
	}
}

void ScreenImage::getTiles(int &rows, int &cols)
{
	rows = m_rows;
	cols = m_cols;
}

int ScreenImage::getRows()
{
	return m_rows;
}

int ScreenImage::getCols()
{
	return m_cols;
}

void ScreenImage::update(int x1, int y1, int x2, int y2)
{
	if (m_image!=nullptr)
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

		int logW = xmax - xmin;
		int logH = ymax - ymin;

		QOpenGLExtraFunctions *f = screenImageGL();
		if (f == nullptr)
			return;

		if (std::abs(m_devicePixelRatio - 1.0) < 0.001)
		{
			// Read the sub-region into a tightly-packed buffer, then copy it into
			// the image. The GL_PACK_ROW_LENGTH/SKIP_PIXELS/SKIP_ROWS trick that
			// lets glReadPixels write the sub-rect straight into the full image
			// buffer works on desktop GL but WebGL 2 rejects it ("readPixels:
			// buffer is not large enough for dimensions"), which silently dropped
			// committed shapes (rect/ellipse/line/arch) on the wasm build.
			std::vector<unsigned char> temp(logW * logH * 4);
			f->glPixelStorei(GL_PACK_ALIGNMENT, 1);
			f->glPixelStorei(GL_PACK_ROW_LENGTH, 0);
			f->glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
			f->glPixelStorei(GL_PACK_SKIP_ROWS, 0);
			f->glReadPixels((int)x + xmin, (int)y + ymin, logW, logH, GL_RGBA, GL_UNSIGNED_BYTE, temp.data());

			for (int row = 0; row < logH; row++)
			{
				for (int col = 0; col < logW; col++)
				{
					int srcIdx = (row * logW + col) * 4;
					int cx = xmin + col;
					int cy = ymin + row;
					m_image->setPixel(cx, cy, temp[srcIdx], temp[srcIdx+1], temp[srcIdx+2]);
					m_image->setPixelAlpha(cx, cy, temp[srcIdx+3]);
				}
			}
		}
		else
		{
			// Read at physical resolution then nearest-neighbour decimate to logical canvas pixels.
			double dpr = m_devicePixelRatio;
			int physX = (int)std::lround((x + xmin) * dpr);
			int physY = (int)std::lround((y + ymin) * dpr);
			int physW = (std::max)(1, (int)std::lround(logW * dpr));
			int physH = (std::max)(1, (int)std::lround(logH * dpr));

			std::vector<unsigned char> temp(physW * physH * 4);
			f->glPixelStorei(GL_PACK_ALIGNMENT, 1);
			f->glPixelStorei(GL_PACK_ROW_LENGTH, 0);
			f->glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
			f->glPixelStorei(GL_PACK_SKIP_ROWS, 0);
			f->glReadPixels(physX, physY, physW, physH, GL_RGBA, GL_UNSIGNED_BYTE, temp.data());

			for (int row = 0; row < logH; row++)
			{
				for (int col = 0; col < logW; col++)
				{
					int srcRow = (std::min)(physH - 1, (std::max)(0, (int)std::floor((row + 0.5) * dpr)));
					int srcCol = (std::min)(physW - 1, (std::max)(0, (int)std::floor((col + 0.5) * dpr)));
					int srcIdx = (srcRow * physW + srcCol) * 4;
					int cx = xmin + col;
					int cy = ymin + row;
					m_image->setPixel(cx, cy, temp[srcIdx], temp[srcIdx+1], temp[srcIdx+2]);
					m_image->setPixelAlpha(cx, cy, temp[srcIdx+3]);
				}
			}
		}

		m_image->fillRectAlpha(xmin, ymin, xmax - 1, ymax - 1, 128);
	}
}

} // namespace ivf2d
