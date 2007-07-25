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

#include "Texture.h"

CTexture::CTexture()
{
	m_bound = false;
	m_image = NULL;
	m_wrapT = GL_REPEAT;
	m_wrapS = GL_REPEAT;
	m_textureMode = GL_DECAL;
	m_magFilter = GL_NEAREST;
	m_minFilter = GL_NEAREST;
	m_textureEnvColor[0] = 1.0;
	m_textureEnvColor[1] = 1.0;
	m_textureEnvColor[2] = 1.0;
	m_textureEnvColor[3] = 1.0;
	m_texScaleX = 1.0;
	m_texScaleY = 1.0;
	m_texRotate = 0.0;
}

CTexture::~CTexture()
{
	if (isBound() == TRUE)
		glDeleteTextures(1, &m_textureName);
}

bool CTexture::isBound()
{
	return m_bound;
}

void CTexture::bind()
{
	if (isBound() == TRUE)
		glDeleteTextures(1, &m_textureName);
	glGenTextures(1, &m_textureName);
	glBindTexture(GL_TEXTURE_2D, m_textureName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrapT);
	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, m_textureMode);
	glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, m_textureEnvColor);

	if (m_image!=NULL)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)m_image->getWidth(), (GLsizei)m_image->getHeight(),
			0, GL_RGB, GL_UNSIGNED_BYTE, m_image->getImageMap());
	}
	
	m_bound = true;
}

void CTexture::apply()
{
	glBindTexture(GL_TEXTURE_2D, m_textureName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrapT);
	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, m_textureMode);
	glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, m_textureEnvColor);
    glMatrixMode (GL_TEXTURE); 
	glLoadIdentity ();
	glRotated(m_texRotate, 0.0, 0.0, 1.0);
    glScaled (m_texScaleX, m_texScaleY, 1.0); 
	glMatrixMode (GL_MODELVIEW);
}

void CTexture::setImage(CImage *image)
{
	m_image = image;
}

CImage* CTexture::getImage()
{
	return m_image;
}
