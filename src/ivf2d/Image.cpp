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

#include "Image.h"

CImage::CImage()
{
	m_imageMap = NULL;
	m_size[0] = -1;
	m_size[1] = -1;
	m_channels = 3;
	m_currentAlpha = 255;
	m_ownData = true;

	m_fillColor[0] = 0;
	m_fillColor[1] = 0;
	m_fillColor[2] = 0;
}

CImage::~CImage()
{
	if (m_ownData)
	{
		if (m_imageMap!=NULL)
			delete [] m_imageMap;
	}
}

void CImage::setSize(int width, int height)
{
	if (m_ownData)
	{
		if ((width>0)&&(height>0))
		{
			m_size[0] = width;
			m_size[1] = height;
			m_ratio = width/height;

			if (m_imageMap!=NULL)
				delete [] m_imageMap;

			m_imageMap = new GLubyte[m_size[0]*m_size[1]*m_channels];
		}
	}
}

int CImage::getWidth()
{
	return m_size[0];
}

int CImage::getHeight()
{
	return m_size[1];
}

GLubyte* CImage::getImageMap()
{
	return m_imageMap;
}

void CImage::setPixel(int x, int y, GLubyte red, GLubyte green, GLubyte blue)
{
	if (valid(x, y))
	{
		setValue(x, y, 0, red);
		setValue(x, y, 1, green);
		setValue(x, y, 2, blue);
		if (m_channels==4)
			setValue(x, y, 3, m_currentAlpha);
	}
}

void CImage::addPixel(int x, int y, GLubyte red, GLubyte green, GLubyte blue)
{
	if (valid(x, y))
	{
		addValue(x, y, 0, red);
		addValue(x, y, 1, green);
		addValue(x, y, 2, blue);
		if (m_channels==4)
			addValue(x, y, 3, m_currentAlpha);
	}
}

void CImage::subtractPixel(int x, int y, GLubyte red, GLubyte green, GLubyte blue)
{
	if (valid(x, y))
	{
		subtractValue(x, y, 0, red);
		subtractValue(x, y, 1, green);
		subtractValue(x, y, 2, blue);
		if (m_channels==4)
			subtractValue(x, y, 3, m_currentAlpha);
	}
}

bool CImage::valid(int x, int y)
{
	return ((x>=0)&&(x<m_size[0])&&(y>=0)&&(y<m_size[1]));
}

void CImage::getPixel(int x, int y, GLubyte &red, GLubyte &green, GLubyte &blue)
{
	if (valid(x, y))
	{
		getValue(x, y, 0, red);
		getValue(x, y, 1, green);
		getValue(x, y, 2, blue);
	}
}

void CImage::clear()
{
	fillColor(0,0,0);
}

void CImage::fillColor(GLubyte red, GLubyte green, GLubyte blue)
{
	int i, j;

	for (i=0; i<m_size[0]; i++)
		for (j=0; j<m_size[1]; j++)
			setPixel(i, j, red, green, blue);
}

void CImage::fillRect(int x1, int y1, int x2, int y2, GLubyte red, GLubyte green, GLubyte blue)
{
	if ( (valid(x1, y1))&&(valid(x2, y2)) )
	{
		int i, j;

		for (i=x1; i<=x2; i++)
			for (j=y1; j<=y2; j++)
				setPixel(i, j, red, green, blue);		
	}
}

double CImage::getRatio()
{
	return m_ratio;
}


void CImage::drawFrame(int x1, int y1, int x2, int y2, GLubyte red, GLubyte green, GLubyte blue)
{
	if ( (valid(x1, y1))&&(valid(x2, y2)) )
	{
		int i;

		for (i=x1; i<=x2; i++)
		{
			setPixel(i, y1, red, green, blue);		
			setPixel(i, y2, red, green, blue);
		}

		for (i=y1; i<=y2; i++)
		{
			setPixel(x1, i, red, green, blue);		
			setPixel(x2, i, red, green, blue);
		}
	}
}

void CImage::setChannels(int number)
{
	if (m_ownData)
	{
		if (number<=3)
			m_channels = 3;
		else
			m_channels = 4;

		this->setSize(m_size[0], m_size[1]);
	}
}

void CImage::setAlpha(GLubyte alpha)
{
	m_currentAlpha = alpha;
}

void CImage::setValue(int x, int y, int channel, GLubyte value)
{
	//	if (valid(x, y))
	//	{
	m_imageMap[x*m_channels + m_size[0]*y*m_channels + channel] = value;
	//	}
}

void CImage::addValue(int x, int y, int channel, GLubyte value)
{
	if ((int)m_imageMap[x*m_channels + m_size[0]*y*m_channels + channel]+(int)value > 255)
		m_imageMap[x*m_channels + m_size[0]*y*m_channels + channel] = 255;
	else
		m_imageMap[x*m_channels + m_size[0]*y*m_channels + channel] += value;
}

void CImage::subtractValue(int x, int y, int channel, GLubyte value)
{
	if ((int)m_imageMap[x*m_channels + m_size[0]*y*m_channels + channel]-(int)value < 0)
		m_imageMap[x*m_channels + m_size[0]*y*m_channels + channel] = 0;
	else
		m_imageMap[x*m_channels + m_size[0]*y*m_channels + channel] -= value;
}


void CImage::createAlphaAll(GLubyte max, GLubyte min)
{
	int i, j;

	GLubyte red, green, blue;

	for (i=0; i<m_size[0]; i++)
	{
		for (j=0; j<m_size[1]; j++)
		{
			getValue(i, j, 0, red);
			getValue(i, j, 1, green);
			getValue(i, j, 2, blue);

			//average = ((double)red + (double)green + (double)blue) / 3.0;

			if (red<5)
				setValue(i, j, 3, 255);
			else
				setValue(i, j, 3, 0);
		}
	}
}

void CImage::getValue(int x, int y, int channel, GLubyte &value)
{
	value = m_imageMap[x*m_channels + m_size[0]*y*m_channels + channel];
}

void CImage::fillAlpha(GLubyte alpha)
{

}

void CImage::createMask(GLubyte comp, GLubyte treshold, GLubyte over, GLubyte under)
{
	int i, j;

	GLubyte red, green, blue, color;

	for (i=0; i<m_size[0]; i++)
	{
		for (j=0; j<m_size[1]; j++)
		{
			getValue(i, j, 0, red);
			getValue(i, j, 1, green);
			getValue(i, j, 2, blue);

			switch (comp) {
			case 0:
				color = red;
				break;
			case 1:
				color = green;
				break;
			case 2:
				color = blue;
				break;
			default:
				break;
			}

			if (color<treshold)
				setValue(i, j, 3, under);
			else
				setValue(i, j, 3, over);
		}
	}
}

void CImage::createAlphaMask(GLubyte min, GLubyte max)
{
	int i, j;

	double average;
	GLubyte red, green, blue;

	for (i=0; i<m_size[0]; i++)
	{
		for (j=0; j<m_size[1]; j++)
		{
			getValue(i, j, 0, red);
			getValue(i, j, 1, green);
			getValue(i, j, 2, blue);

			average = ((double)red + (double)green + (double)blue) / 3.0;

			if (red>0)
				setValue(i, j, 3, min);
			else
				setValue(i, j, 3, max);
		}
	}
}

void CImage::colorAlpha(GLubyte red, GLubyte green, GLubyte blue)
{
	int i, j;

	double dVal;
	GLubyte alpha;

	for (i=0; i<m_size[0]; i++)
	{
		for (j=0; j<m_size[1]; j++)
		{
			getValue(i, j, 3, alpha);

			if (alpha>0)
			{
				dVal = (double)alpha/255.0;
				setValue(i, j, 0, (255-alpha) + (GLubyte)((double)red*dVal));
				setValue(i, j, 1, (255-alpha) + (GLubyte)((double)green*dVal));
				setValue(i, j, 2, (255-alpha) + (GLubyte)((double)blue*dVal));
			}
			else
			{

			}
		}
	}
}

void CImage::invert()
{
	int i, j;
	GLubyte red, green, blue;

	for (i=0; i<m_size[0]; i++)
		for (j=0; j<m_size[1]; j++)
		{
			getValue(i, j, 0, red);
			getValue(i, j, 1, green);
			getValue(i, j, 2, blue);
			setPixel(i, j, 255-red, 255-green, 255-blue);
		}
}


void CImage::doFloodFill(int x, int y)
{
	if (validPixel(x, y))
	{
		setPixel(x, y, m_fillColor[0], m_fillColor[1], m_fillColor[2]);

		if (validPixel(x-1, y-1)) pushNextPixel(x-1, y-1);
		if (validPixel(x-1, y))	 pushNextPixel(x-1, y);
		if (validPixel(x-1, y+1)) pushNextPixel(x-1, y+1);
		if (validPixel(x, y-1))   pushNextPixel(x, y-1);
		if (validPixel(x, y+1))   pushNextPixel(x, y+1);
		if (validPixel(x+1, y-1)) pushNextPixel(x+1, y-1);
		if (validPixel(x+1, y))   pushNextPixel(x+1, y);
		if (validPixel(x+1, y+1)) pushNextPixel(x+1, y+1);
	}
}

void CImage::pushPixel(int x, int y)
{
	m_processListX.push_back(x);
	m_processListY.push_back(y);
}

bool CImage::popPixel(int &x, int &y)
{
	if (!m_processListX.empty())
	{
		x = m_processListX.back();
		y = m_processListY.back();
		m_processListX.pop_back();
		m_processListY.pop_back();
		return true;
	}
	else
		return false;
}

void CImage::pushNextPixel(int x, int y)
{
	m_nextListX.push_back(x);
	m_nextListY.push_back(y);
}

bool CImage::popNextPixel(int &x, int &y)
{

	if (!m_nextListX.empty())
	{
		x = m_nextListX.back();
		y = m_nextListY.back();
		m_nextListX.pop_back();
		m_nextListY.pop_back();
		return true;
	}
	else
		return false;
}

void CImage::floodFill(int x, int y)
{
	m_processListX.clear();
	m_processListY.clear();

	GLubyte red, green, blue;

	getValue(x, y, 0, red);
	getValue(x, y, 1, green);
	getValue(x, y, 2, blue);

	m_startColor[0] = red;
	m_startColor[1] = green;
	m_startColor[2] = blue;

	if ((m_startColor[0]==m_fillColor[0])&&
		(m_startColor[1]==m_fillColor[1])&&
		(m_startColor[2]==m_fillColor[2]))
	{
		return;
	}

	pushPixel(x, y);

	int xx, yy;

	do
	{
		m_nextListX.clear();
		m_nextListY.clear(); 

		while (popPixel(xx, yy)) 
			doFloodFill(xx, yy);

		m_processListX.clear();
		m_processListY.clear();

		m_processListX = m_nextListX;
		m_processListY = m_nextListY;

	} while (!m_processListX.empty());
}

bool CImage::validPixel(int x, int y)
{
	if (valid(x, y))
	{
		GLubyte red, green, blue;

		getValue(x, y, 0, red);
		getValue(x, y, 1, green);
		getValue(x, y, 2, blue);

		return ((red==m_startColor[0])&&(green==m_startColor[1])&&(blue==m_startColor[2]));
	}
	else
		return false;
}

void CImage::setFillColor(GLubyte red, GLubyte green, GLubyte blue)
{
	m_fillColor[0] = red;
	m_fillColor[1] = green;
	m_fillColor[2] = blue;
}

void CImage::copyFrom(CImage *image)
{
	int i, j;
	int w, h;
	GLubyte red, green, blue;

	w = image->getWidth();
	h = image->getHeight();

	for (i=0; i<w; i++)
		for (j=0; j<h; j++)
		{
			if ((i<m_size[0])&&(j<m_size[1]))
			{
				image->getPixel(i, j, red, green, blue);
				setPixel(i, j, red, green, blue);
			}
		}
}

void CImage::copyFrom(CImage* image, int startx, int starty)
{
	int i, j;
	int w, h;
	GLubyte red, green, blue;

	w = image->getWidth();
	h = image->getHeight();

	if ((startx>=0)&&(startx<this->getWidth())&&(starty>=0)&&(starty<this->getHeight()))
	{
		for (i=0; i<w; i++)
			for (j=0; j<h; j++)
			{
				if ((i<m_size[0])&&(j<m_size[1]))
				{
					image->getPixel(i, j, red, green, blue);
					subtractPixel(startx + i, starty + j, 255-red, 255-green, 255-blue);
				}
			}
	}
}

void CImage::copyFrom(CImage* image, int startx, int starty, const float* color)
{
	int i, j;
	int w, h;
	GLubyte red, green, blue;

	w = image->getWidth();
	h = image->getHeight();

	if ((startx>=0)&&(startx<this->getWidth())&&(starty>=0)&&(starty<this->getHeight()))
	{
		for (i=0; i<w; i++)
			for (j=0; j<h; j++)
			{
				if ((i<m_size[0])&&(j<m_size[1]))
				{
					image->getPixel(i, j, red, green, blue);
					//cout << "raw: " << (int)red << ", " << (int)green << ", " << (int)blue << endl;
					if ((red!=255)&&(green!=255)&&(blue!=255))
					{
						red = (GLubyte)(color[0]*255);
						green = (GLubyte)(color[1]*255);
						blue = (GLubyte)(color[2]*255);
						setPixel(startx + i, starty + j, red, green, blue);
						//cout << "processed: " << (int)red << ", " << (int)green << ", " << (int)blue << endl;
					}
				}
			}
	}
}


void* CImage::getData()
{
	return (void*)m_imageMap;
}

void CImage::setImageMap(int width, int height, GLubyte* data, bool ownData)
{
	if (m_ownData)
	{
		if (m_imageMap!=NULL)
			delete [] m_imageMap;
	}

	m_ownData = ownData;
	m_imageMap = (GLubyte*)data;
	m_size[0] = width;
	m_size[1] = height;
	m_channels = 3;
}

int CImage::getChannels()
{
	return m_channels;
}

void CImage::grayscale()
{
	int i, j;
	GLubyte red, green, blue, byteMedian;
	double median;

	for (i=0; i<m_size[0]; i++)
		for (j=0; j<m_size[1]; j++)
		{
			getValue(i, j, 0, red);
			getValue(i, j, 1, green);
			getValue(i, j, 2, blue);

			median = ((double)red + (double)green + (double)blue) / 3.0;
			byteMedian = (GLubyte) median;
			setPixel(i, j, byteMedian, byteMedian, byteMedian);
		}
}

void CImage::drawImage(int x, int y, CImage *image)
{
	int i, j;
	int w, h;
	GLubyte sred, sgreen, sblue;
	GLubyte tred, tgreen, tblue;

	w = image->getWidth();
	h = image->getHeight();

	for (i=0; i<w; i++)
		for (j=0; j<h; j++)
		{
			if (((x+i)<m_size[0])&&((y+j)<m_size[1]))
			{
				image->getPixel(i, j, sred, sgreen, sblue);
				this->getPixel(x+i, y+j, tred, tgreen, tblue);
				this->setPixel(x+i, y+j, sred|!tred, sgreen|!tgreen, sblue|!tblue);

				/*
				if ((sred<255)||(sgreen<255)||(sblue<255))
				setPixel(x+i, y+j, sred, sgreen, sblue);
				*/
			}
		}
}
