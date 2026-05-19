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

#ifndef _PngImage_h_
#define _PngImage_h_

#include "Image.h"

namespace ivf2d {

IvfSmartPointer(PngImage);

/**
 * PNG file loader 
 *
 * This class loads a png image using the 
 * libpng library.
 *
 * @author Jonas Lindemann
 */
class PngImage : public Image {
private:
	std::string m_fileName;
public:
	/** CIvfPngImage constructor */
	PngImage();

	/** CIvfPngImage destructor */
	virtual ~PngImage();

	static PngImagePtr create() { return std::make_shared<PngImage>(); }

	IvfClassInfo("PngImage",Image);

	/** Reads PNG image */
	virtual bool read();

	void setFileName(const std::string& name);
	const std::string& getFileName();
};


} // namespace ivf2d

#endif 
