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
#ifndef _ScreenImage_h_
#define _ScreenImage_h_

#include "Shape.h"
#include "Image.h"

namespace ivf2d {

IvfSmartPointer(ScreenImage);

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

};


} // namespace ivf2d

#endif 
