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
	int m_layers;
	int m_currentLayer;
	int m_size[2];
	double m_ratio;
    unsigned char* m_imageMap;
    unsigned char** m_imageMaps;
	int m_channels;
    unsigned char m_currentAlpha;
    unsigned char m_startColor[3];
    unsigned char m_fillColor[3];
	deque<int> m_nextListX;
	deque<int> m_nextListY;
	deque<int> m_processListX;
	deque<int> m_processListY;
	bool m_ownData;

	void initLayers();
	void clearLayers();
	void destroyLayers();
public:
	CImage();
	CImage(int nLayers);
	virtual ~CImage();

	IvfClassInfo("CImage",CBase);

	// Methods

	/** Draw a rectangular frame. */
    void drawFrame(int x1, int y1, int x2, int y2, unsigned char red, unsigned char green, unsigned char blue);

	/** Draws a filled rectangular frame. */
    void fillRect(int x1, int y1, int x2, int y2, unsigned char red, unsigned char green, unsigned char blue);

	/** Fills 2D image with specified color. */
    void fillColor(unsigned char red, unsigned char green, unsigned char blue);

	/** Clears image. Color is set to black. */
	void clear();

	/** Returns true if position (\c x, \c y) is a valid image pixel. */
	bool valid(int x, int y);

	/** Sets the pixel at position (\c x, \c y) to a specified color. */
    void setPixel(int x, int y, unsigned char red, unsigned char green, unsigned char blue);
    void addPixel(int x, int y, unsigned char red, unsigned char green, unsigned char blue);
    void setPixelAlpha(int x, int y, unsigned char alpha);
    void subtractPixel(int x, int y, unsigned char red, unsigned char green, unsigned char blue);

	/** Retrieve pixel color at position (\c x, \c y). */
    void getPixel(int x, int y, unsigned char &red, unsigned char &green, unsigned char &blue);

	void drawImage(int x, int y, CImage* image);
	void drawImageLine(CImage* image, int x1, int y1, int x2, int y2, float* color);
	void grayscale();
	int getChannels();
    void setImageMap(int width, int height, unsigned char* data, bool ownData);
	void* getData();
	void copyFrom(CImage* image);
	void copyFrom(CImage* image, int startx, int starty);
	void copyFrom(CImage* image, int startx, int starty, const float* color);
    void setFillColor(unsigned char red, unsigned char green, unsigned char blue);
	bool validPixel(int x, int y);
	void floodFill(int x, int y);
	bool popNextPixel(int &x, int &y);
	void pushNextPixel(int x, int y);
	bool popPixel(int &x, int &y);
	void pushPixel(int x, int y);
	void doFloodFill(int x, int y);
	void invert();
    void colorAlpha(unsigned char red, unsigned char green, unsigned char blue);
    void createAlphaMask(unsigned char min, unsigned char max);
    void createMask(unsigned char comp, unsigned char treshold, unsigned char over, unsigned char under);
    void fillAlpha(unsigned char alpha);
    void getValue(int x, int y, int channel, unsigned char &value);
    void createAlphaAll(unsigned char max, unsigned char min);
    void fillRectAlpha(int x1, int y1, int x2, int y2, unsigned char alpha);

	// Get/set methods

	/** Set size of image. This will erase any previous image. */
	void setSize(int width, int height);

	/** Return height of image in pixels. */
	int getHeight();

	/** Return width of image in pixels. */
	int getWidth();

	/** Return image ratio width/height. */
	double getRatio();

	void setLayer(int layer);
	int getLayer();

	int getLayerCount();

	/** 
	 * Return pointer to image map. 
	 * 
	 * This is typically used when specifying a texture in OpenGL. 
	 */
    unsigned char* getImageMap();

    void setAlpha(unsigned char alpha);
    unsigned char getAlpha();
	void setChannels(int number);
    void setValue(int x, int y, int channel, unsigned char value);
    void addValue(int x, int y, int channel, unsigned char value);
    void subtractValue(int x, int y, int channel, unsigned char value);
};

#endif 
