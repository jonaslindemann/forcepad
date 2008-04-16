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

#include "JpegImage.h"

extern "C" {
#include "jpeglib.h"
}

CJpegImage::CJpegImage()
{
	m_fileName = NULL;
}

CJpegImage::~CJpegImage()
{
	if (m_fileName!=NULL)
		delete [] m_fileName;	
}

void CJpegImage::setFileName(const char *name)
{
	if (m_fileName!=NULL)
		delete [] m_fileName;

	m_fileName = new char[strlen(name)+1];
	strcpy(m_fileName, name);
}

const char* CJpegImage::getFileName()
{
	return m_fileName;
}

bool CJpegImage::read()
{
	struct jpeg_decompress_struct cinfo;
	FILE * infile;		
	JSAMPARRAY buffer;	
	int row_stride;		
	struct jpeg_error_mgr pub;
	
	if ((infile = fopen(getFileName(), "rb")) == NULL) {
		fprintf(stderr, "can't open %s\n", getFileName());
		return 0;
	}

    cinfo.err = jpeg_std_error(&pub);
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, infile);

	try {
		jpeg_read_header(&cinfo, TRUE);
	}
	catch (...) {
		jpeg_destroy_decompress(&cinfo);		
		fclose(infile);
		return false;
	}
	
	jpeg_start_decompress(&cinfo);
	
	this->setChannels(cinfo.output_components);
	this->setSize(cinfo.output_width, cinfo.output_height);

	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
	
	while (cinfo.output_scanline < cinfo.output_height) {
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);
		unsigned int i, j;

		for (i=0; i<cinfo.output_width; i++)
			for (j=0; j<(unsigned int)cinfo.output_components; j++)
				this->setValue(i, cinfo.output_height - cinfo.output_scanline, j, (GLubyte)buffer[0][i*cinfo.output_components+j]);
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	
	fclose(infile);
	
	return 1;
}
