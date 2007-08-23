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

#include "Clipboard.h"

CClipboard::CClipboard()
{
	m_baseImage = NULL;
	m_clipboard = new CImage();

	m_pasteMode = PM_NON_WHITE;
	m_copyImageMode = IM_RGB;

	m_selectionX[0] = 0.0;
	m_selectionX[1] = 0.0;
	m_selectionY[0] = 0.0;
	m_selectionY[1] = 0.0;
}

CClipboard::~CClipboard()
{
}

void CClipboard::copy(int x1, int y1, int x2, int y2)
{
	if (m_baseImage!=NULL)
	{
		int w = x2 - x1 + 1;
		int h = y2 - y1 + 1;

		m_selectionX[0] = x1;
		m_selectionX[1] = x2;
		m_selectionY[0] = y1;
		m_selectionY[1] = y2;

		if ((w>0)&&(h>0))
		{
			m_clipboard->setSize(w, h);

			int i, j;

			GLubyte red, green, blue;

			for (j=y1; j<=y2; j++)
				for (i=x1; i<=x2; i++)
				{
					m_baseImage->getPixel(i, j, red, green, blue);
					m_clipboard->setPixel(i-x1, j-y1, red, green, blue);
				}
		}
	}
}

void CClipboard::cut(int x1, int y1, int x2, int y2)
{
	if (m_baseImage!=NULL)
	{
		int w = x2 - x1 + 1;
		int h = y2 - y1 + 1;

		m_selectionX[0] = x1;
		m_selectionX[1] = x2;
		m_selectionY[0] = y1;
		m_selectionY[1] = y2;

		if ((w>0)&&(h>0))
		{
			m_clipboard->setSize(w, h);

			int i, j;

			GLubyte red, green, blue;

			for (j=y1; j<=y2; j++)
				for (i=x1; i<=x2; i++)
				{
					m_baseImage->getPixel(i, j, red, green, blue);
					m_baseImage->setPixel(i, j, 255, 255, 255);
					m_clipboard->setPixel(i-x1, j-y1, red, green, blue);
				}
		}
	}
}

void CClipboard::paste(int x, int y)
{
	if (m_baseImage!=NULL)
	{
		int w = m_clipboard->getWidth();
		int h = m_clipboard->getHeight();

		if ((w>0)&&(h>0))
		{
			int i, j;

			GLubyte red, green, blue;

			for (j=0; j<h; j++)
				for (i=0; i<=w; i++)
				{
					if ((x+i<m_baseImage->getWidth())&&(y+j<m_baseImage->getHeight()))
					{
						m_clipboard->getPixel(i, j, red, green, blue);
						if (m_pasteMode==PM_NON_WHITE)
						{
							if ((red!=255)&&(green!=255)&&(blue!=255))
								m_baseImage->setPixel(x+i, y+j, red, green, blue);
						}
						else
							m_baseImage->setPixel(x+i, y+j, red, green, blue);
					}
				}
		}
	}
}

CImage* CClipboard::getClipboard()
{
	return m_clipboard;
}

void CClipboard::setImage(CImage *image)
{
	m_baseImage = image;
}

void CClipboard::setPasteMode(TPasteMode mode)
{
	m_pasteMode = mode;
}


void CClipboard::copyImage(int width, int height, GLubyte *imageMap)
{
	CImage* image = new CImage();
	image->setImageMap(width, height, imageMap, false);
	
	//int w = x2 - x1 + 1;
	//int h = y2 - y1 + 1;
	
	if ((width>0)&&(height>0))
	{
		int i, j;
		GLubyte red, green, blue, byteMedian;
		double median;

		m_clipboard->setSize(width, height);
		m_clipboard->clear();
		
		for (j=0; j<height; j++)
			for (i=0; i<width; i++)
			{
				switch (m_copyImageMode) {
				case IM_RGB:
					image->getPixel(i, j, red, green, blue);
					break;
				case IM_RBG:
					image->getPixel(i, j, red, blue, green);
					break;
				case IM_BGR:
					image->getPixel(i, j, blue, green, red);
					break;
				case IM_BRG:
					image->getPixel(i, j, blue, red, green);
					break;
				case IM_GRB:
					image->getPixel(i, j, green, red, blue);
					break;
				case IM_GBR:
					image->getPixel(i, j, green, blue, red);
					break;
				case IM_GRAYSCALE:
					image->getPixel(i, j, red, green, blue);

					median = ((double)red + (double)green + (double)blue) / 3.0;
					byteMedian = (GLubyte) median;

					red = byteMedian;
					green = byteMedian;
					blue = byteMedian;

					break;
				default:
					image->getPixel(i, j, red, green, blue);
					break;
				}
				m_clipboard->setPixel(i, j, red, green, blue);
			}
	}

	delete image;
}

void CClipboard::setCopyImageMode(TCopyImageMode mode)
{
	m_copyImageMode = mode;
}

void CClipboard::getSelection(int &x1, int &y1, int &x2, int &y2)
{
	x1 = m_selectionX[0];
	y1 = m_selectionY[0];
	x2 = m_selectionX[1];
	y2 = m_selectionY[1];
}
