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

#pragma once

#include <memory>

#include "Shape.h"
#include "Image.h"

namespace ivf2d {

IvfSmartPointer(ScreenImage);

// Forward-declared so the modern-GL header (and its Qt include) stays out of
// ScreenImage.h, which is pulled into framework-agnostic consumers.
class StreamTexture;

class ScreenImage : public Shape {
public:
	enum TRenderMode {
		RM_NORMAL,
		RM_SUBIMAGE,
		RM_TILED
	};
private:
	ImagePtr m_image;
	int m_subImagePos[2];
	int m_subImageSize[2];
	TRenderMode m_renderMode;
	int m_rows;
	int m_cols;
	int m_tileSpacing[2];
	double m_devicePixelRatio;
	std::unique_ptr<StreamTexture> m_streamTexture;
public:
	ScreenImage();
	virtual ~ScreenImage();

	static ScreenImagePtr create() { return std::make_shared<ScreenImage>(); }

	IvfClassInfo("ScreenImage",Shape);

	void reset();
	void update(int x1, int y1, int x2, int y2);

	void setTiles(int rows, int cols);
	void getTiles(int &rows, int &cols);

	int getRows();
	int getCols();

	TRenderMode getRenderMode();
	void setRenderMode(TRenderMode mode);

	void setImage(ImagePtr image);
	void getSubImageSize(int &width, int &height);
	void setSubImageSize(int width, int height);

	void setDevicePixelRatio(double dpr);

	virtual void doGeometry();

protected:
	// The image is positioned and drawn entirely by doGeometry() via Renderer2D,
	// so the legacy raster-position / matrix-stack transforms in Shape are
	// bypassed (they do not exist in a core / WebGL profile).
	virtual void doBeginTransform() override {}
	virtual void doEndTransform() override {}

};


} // namespace ivf2d
