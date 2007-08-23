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

#include "PngImage.h"

#include "png.h"

CPngImage::CPngImage()
{
	m_fileName = NULL;	
}

CPngImage::~CPngImage()
{
	if (m_fileName!=NULL)
		delete [] m_fileName;
}

void CPngImage::setFileName(const char *name)
{
	if (m_fileName!=NULL)
		delete [] m_fileName;

	m_fileName = new char[strlen(name)+1];
	strcpy(m_fileName, name);
}

const char* CPngImage::getFileName()
{
	return m_fileName;
}

bool CPngImage::read()
{
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	png_uint_32 width, height;
	int bit_depth, color_type;
	FILE *fp;
	
	if ((fp = fopen(getFileName(), "rb")) == NULL)
		return false;
	
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
		NULL, NULL, NULL);
	
	if (png_ptr == NULL)
	{
		fclose(fp);
		return false;
	}
	
	/* Allocate/initialize the memory for image information.  REQUIRED. */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		fclose(fp);
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		return false;
	}
	
	/* Set up the input control if you are using standard C streams */
	png_init_io(png_ptr, fp);
	
	/* The easiest way to read the image: */
	png_bytep* row_pointers;
	
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
	row_pointers = png_get_rows(png_ptr, info_ptr);
	
    width            = png_get_image_width(png_ptr, info_ptr);
    height           = png_get_image_height(png_ptr, info_ptr);
    bit_depth        = png_get_bit_depth(png_ptr, info_ptr);
    color_type       = png_get_color_type(png_ptr, info_ptr);

	bool supported = true;

	switch (color_type) {
	case PNG_COLOR_TYPE_GRAY:
		setChannels(1);
		break;
	case PNG_COLOR_TYPE_GRAY_ALPHA:
		setChannels(2);
		break;
	case PNG_COLOR_TYPE_PALETTE:
		supported = false;
		break;
	case PNG_COLOR_TYPE_RGB:
		setChannels(3);
		break;
	case PNG_COLOR_TYPE_RGB_ALPHA:
		setChannels(4);
		break;
	case PNG_COLOR_MASK_PALETTE:
		supported = false;
		break;
	default:
		supported = false;
		break;
	}

	if (!supported)
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		return false;
	}

	setSize(width, height);

	unsigned int i, j, k;

	for (i=0; i<height; i++)
		for (j=0; j<width; j++)
			for (k=0; k<getChannels(); k++)
				setValue(j, height-i-1, k, row_pointers[i][j*getChannels()+k]);

	
	/* At this point you have read the entire image */
	
	/* clean up after the read, and free any memory allocated - REQUIRED */
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
	
	/* close the file */
	fclose(fp);
	
	/* that's it */
	return true;
}
