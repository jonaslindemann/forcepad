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

#ifndef _CImage_h_
#define _CImage_h_

#include "Base.h"
#include "DrawableBase.h" // OpenGL stuff

IvfSmartPointer(CImage);

/**
 * Image class
 *
 * The image class handles a general 2D color image, which
 * can be used in conjunction with the Texture class.
 */
class CImage : public CBase {
private:
	int m_size[2];
	double m_ratio;
	GLubyte* m_imageMap;
	int m_channels;
	GLubyte m_currentAlpha;
	GLubyte m_startColor[3];
	GLubyte m_fillColor[3];
	deque<int> m_nextListX;
	deque<int> m_nextListY;
	deque<int> m_processListX;
	deque<int> m_processListY;
	bool m_ownData;
public:
	void drawImage(int x, int y, CImage* image);
	void grayscale();
	int getChannels();
	void setImageMap(int width, int height, GLubyte* data, bool ownData);
	void* getData();
	void copyFrom(CImage* image);
	void copyFrom(CImage* image, int startx, int starty);
	void copyFrom(CImage* image, int startx, int starty, const float* color);
	void setFillColor(GLubyte red, GLubyte green, GLubyte blue);
	bool validPixel(int x, int y);
	void floodFill(int x, int y);
	bool popNextPixel(int &x, int &y);
	void pushNextPixel(int x, int y);
	bool popPixel(int &x, int &y);
	void pushPixel(int x, int y);
	void doFloodFill(int x, int y);
	void invert();
	void colorAlpha(GLubyte red, GLubyte green, GLubyte blue);
	void createAlphaMask(GLubyte min, GLubyte max);
	void createMask(GLubyte comp, GLubyte treshold, GLubyte over, GLubyte under);
	void fillAlpha(GLubyte alpha);
	void getValue(int x, int y, int channel, GLubyte &value);
	void createAlphaAll(GLubyte max, GLubyte min);

	CImage();
	virtual ~CImage();

	IvfClassInfo("CImage",CBase);

	// Methods

	/** Draw a rectangular frame. */
	void drawFrame(int x1, int y1, int x2, int y2, GLubyte red, GLubyte green, GLubyte blue);

	/** Draws a filled rectangular frame. */
	void fillRect(int x1, int y1, int x2, int y2, GLubyte red, GLubyte green, GLubyte blue);

	/** Fills 2D image with specified color. */
	void fillColor(GLubyte red, GLubyte green, GLubyte blue);

	/** Clears image. Color is set to black. */
	void clear();

	/** Returns true if position (\c x, \c y) is a valid image pixel. */
	bool valid(int x, int y);

	/** Sets the pixel at position (\c x, \c y) to a specified color. */
	void setPixel(int x, int y, GLubyte red, GLubyte green, GLubyte blue);
	void addPixel(int x, int y, GLubyte red, GLubyte green, GLubyte blue);
	void subtractPixel(int x, int y, GLubyte red, GLubyte green, GLubyte blue);

	/** Retrieve pixel color at position (\c x, \c y). */
	void getPixel(int x, int y, GLubyte &red, GLubyte &green, GLubyte &blue);

	// Get/set methods

	/** Set size of image. This will erase any previous image. */
	void setSize(int width, int height);

	/** Return height of image in pixels. */
	int getHeight();

	/** Return width of image in pixels. */
	int getWidth();

	/** Return image ratio width/height. */
	double getRatio();

	/** 
	 * Return pointer to image map. 
	 * 
	 * This is typically used when specifying a texture in OpenGL. 
	 */
	GLubyte* getImageMap();

	void setAlpha(GLubyte alpha);
	void setChannels(int number);
	void setValue(int x, int y, int channel, GLubyte value);
	void addValue(int x, int y, int channel, GLubyte value);
	void subtractValue(int x, int y, int channel, GLubyte value);
};

#endif 
