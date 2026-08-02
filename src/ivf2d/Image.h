//
// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2026 Division of Structural Mecahnics, Lund University
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

#pragma once

#include "Base.h"
#include "DrawableBase.h"
#include <deque>
#include <memory>
#include <vector>

namespace ivf2d {

class Image;
using ImagePtr = std::shared_ptr<Image>;

/**
 * Image class
 *
 * The image class handles a general 2D color image. It is the CPU-side pixel
 * buffer that StreamTexture uploads to the GPU.
 */
class Image : public Base {
private:
	int m_layers{1};
	int m_currentLayer{0};
	int m_size[2]{-1, -1};
	int m_channels{3};

	/**
	 * Owned pixel storage, one buffer per layer. Empty when m_ownData is false,
	 * i.e. when setImageMap() has pointed this image at a caller-owned buffer.
	 */
	std::vector<std::vector<unsigned char>> m_layerData;

	/**
	 * Points at the current layer's pixels -- either into m_layerData or at the
	 * external buffer handed to setImageMap(). Non-owning either way.
	 */
	unsigned char* m_imageMap{nullptr};

	unsigned char m_currentAlpha{255};
	unsigned char m_startColor[3]{};
	unsigned char m_fillColor[3]{};
	std::deque<int> m_nextListX;
	std::deque<int> m_nextListY;
	std::deque<int> m_processListX;
	std::deque<int> m_processListY;
	bool m_ownData{true};

	/** Points m_imageMap at layer \c layer of m_layerData, or null if unsized. */
	void pointAtLayer(int layer);
public:
	Image() = default;
	explicit Image(int nLayers);
	~Image() override = default;

	// Owns heap pixel data through m_layerData; copying was never supported by
	// the old raw-pointer implementation (it would have double-freed), so the
	// copy operations stay deleted rather than silently becoming valid.
	Image(const Image &) = delete;
	Image &operator=(const Image &) = delete;

	static ImagePtr create() { return std::make_shared<Image>(); }
	static ImagePtr create(int nLayers) { return std::make_shared<Image>(nLayers); }

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
	bool valid(int x, int y) const;

	/** Sets the pixel at position (\c x, \c y) to a specified color. */
    void setPixel(int x, int y, unsigned char red, unsigned char green, unsigned char blue);
    void addPixel(int x, int y, unsigned char red, unsigned char green, unsigned char blue);
    void setPixelAlpha(int x, int y, unsigned char alpha);
    void subtractPixel(int x, int y, unsigned char red, unsigned char green, unsigned char blue);

	/** Retrieve pixel color at position (\c x, \c y). */
    void getPixel(int x, int y, unsigned char &red, unsigned char &green, unsigned char &blue) const;

	void drawImage(int x, int y, Image* image);
	void drawImageLine(Image* image, int x1, int y1, int x2, int y2, float* color);
	void grayscale();
	int getChannels() const;
    void setImageMap(int width, int height, unsigned char* data, bool ownData);
	void copyFrom(Image* image);
	void copyFrom(Image* image, int startx, int starty);
	void copyFrom(Image* image, int startx, int starty, const float* color);
    void setFillColor(unsigned char red, unsigned char green, unsigned char blue);
	bool validPixel(int x, int y) const;
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
    void getValue(int x, int y, int channel, unsigned char &value) const;
    void fillRectAlpha(int x1, int y1, int x2, int y2, unsigned char alpha);

	// Get/set methods

	/** Set size of image. This will erase any previous image. */
	void setSize(int width, int height);

	/** Return height of image in pixels. */
	int getHeight() const;

	/** Return width of image in pixels. */
	int getWidth() const;

	void setLayer(int layer);
	int getLayer() const;

	int getLayerCount() const;

	/** 
	 * Return pointer to image map. 
	 * 
	 * This is typically used when specifying a texture in OpenGL. 
	 */
    unsigned char* getImageMap();

    void setAlpha(unsigned char alpha);
    unsigned char getAlpha() const;
	void setChannels(int number);
    void setValue(int x, int y, int channel, unsigned char value);
    void addValue(int x, int y, int channel, unsigned char value);
    void subtractValue(int x, int y, int channel, unsigned char value);
};


} // namespace ivf2d
