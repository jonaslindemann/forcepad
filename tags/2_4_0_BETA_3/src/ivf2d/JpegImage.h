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

#ifndef _CJpegImage_h_
#define _CJpegImage_h_

#include "Image.h"

IvfSmartPointer(CJpegImage);

/**
 * Jpeg file loader 
 *
 * This class loads a jpeg image using the 
 * libjpeg library.
 *
 * @author Jonas Lindemann
 */
class CJpegImage : public CImage {
private:
	char* m_fileName;
public:
	/** CIvfJpegImage constructor */
	CJpegImage();

	/** CIvfJpegImage destructor */
	virtual ~CJpegImage();

	IvfClassInfo("CJpegImage",CImage);

	/** Reads jpeg image */
	virtual bool read();

	void setFileName(const char* name);
	const char* getFileName();

};

#endif 
