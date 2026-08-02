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
#include "Vec2d.h"
#include "Image.h"
#include <memory>

namespace ivf2d {

class Clipboard;
using ClipboardPtr = std::shared_ptr<Clipboard>;

class Clipboard : public Base {
public:
	enum class TPasteMode {
		PM_REPLACE,
		PM_NON_WHITE
	};

	enum class TCopyImageMode {
		IM_RGB,
		IM_RBG,
		IM_BGR,
		IM_BRG,
		IM_GRB,
		IM_GBR,
		IM_GRAYSCALE
	};
private:
	ImagePtr m_baseImage;
	ImagePtr m_clipboard;
	TPasteMode m_pasteMode;
	TCopyImageMode m_copyImageMode;
	int m_selectionX[2];
	int m_selectionY[2];
public:
	Clipboard();
	virtual ~Clipboard();

	static ClipboardPtr create() { return std::make_shared<Clipboard>(); }

	virtual void copy(int x1, int y1, int x2, int y2);
	virtual void cut(int x1, int y1, int x2, int y2);
	virtual void paste(int x, int y);
	
    void copyImage(int width, int height, unsigned char* imageMap);

	void setPasteMode(TPasteMode mode);
	void setImage(ImagePtr image);

	/**
	 * Non-owning observer of the clipboard image. The Clipboard keeps the
	 * shared_ptr; callers must not delete the result or hold it past the next
	 * copy()/cut()/setImage() call.
	 */
	Image* getClipboardImage();

	void setCopyImageMode(TCopyImageMode mode);
	Rect2i getSelection() const;

	/** Non-owning observer -- same contract as getClipboardImage(). */
	Image* getClipboard();
};


} // namespace ivf2d
